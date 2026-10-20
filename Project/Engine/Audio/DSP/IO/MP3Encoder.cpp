#include "MP3Encoder.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

using Microsoft::WRL::ComPtr;

MP3Encoder::MP3Encoder() : MP3Encoder(Param{}) {}

MP3Encoder::MP3Encoder(const Param& param) : param_(param) {
    // MFStartup/MFShutdownは参照カウント式なので、Audio側の初期化とは独立して呼んでよい
    isMediaFoundationStarted_ = SUCCEEDED(MFStartup(MF_VERSION));
}

MP3Encoder::~MP3Encoder() {
    if (isMediaFoundationStarted_) {
        MFShutdown();
    }
}

bool MP3Encoder::IsSupported(const AudioPCM& pcm) const {
    const int rate = pcm.GetSampleRate();
    const int ch   = pcm.GetChannelCount();
    return (rate == 32000 || rate == 44100 || rate == 48000) && (ch == 1 || ch == 2);
}

bool MP3Encoder::Encode(const AudioPCM& pcm, const std::string& filePath) {
    lastError_.clear();
    if (!isMediaFoundationStarted_) {
        return Fail("Media Foundationの初期化に失敗しています");
    }
    if (pcm.IsEmpty()) {
        return Fail("波形が空です");
    }
    if (!IsSupported(pcm)) {
        return Fail("mp3は32000/44100/48000Hz・1～2chのみ対応です(現在 " + std::to_string(pcm.GetSampleRate()) + "Hz / "
                    + std::to_string(pcm.GetChannelCount()) + "ch)");
    }

    // 16bitPCMへ
    WAVEFORMATEX format{};
    std::vector<BYTE> bytes;
    converter_.Encode16(pcm, format, bytes);

    // 出力先フォルダ作成とパスのワイド文字化
    const std::filesystem::path path(reinterpret_cast<const char8_t*>(filePath.c_str()));
    std::error_code ec;
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path(), ec);
    }

    // SinkWriter作成(コンテナをmp3に指定)
    ComPtr<IMFAttributes> attributes;
    HRESULT hr = MFCreateAttributes(&attributes, 1);
    if (SUCCEEDED(hr)) {
        hr = attributes->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MP3);
    }
    ComPtr<IMFSinkWriter> writer;
    if (SUCCEEDED(hr)) {
        hr = MFCreateSinkWriterFromURL(path.wstring().c_str(), nullptr, attributes.Get(), &writer);
    }
    if (FAILED(hr)) {
        return Fail("SinkWriterの作成に失敗しました: " + filePath);
    }

    // 出力形式(mp3)
    ComPtr<IMFMediaType> outputType;
    hr = MFCreateMediaType(&outputType);
    if (SUCCEEDED(hr)) hr = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    if (SUCCEEDED(hr)) hr = outputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
    if (SUCCEEDED(hr)) hr = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, format.nChannels);
    if (SUCCEEDED(hr)) hr = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, format.nSamplesPerSec);
    if (SUCCEEDED(hr)) hr = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, param_.bitrateKbps * 1000 / 8);

    DWORD streamIndex = 0;
    if (SUCCEEDED(hr)) hr = writer->AddStream(outputType.Get(), &streamIndex);
    if (FAILED(hr)) {
        return Fail("mp3の出力形式が設定できません(ビットレート " + std::to_string(param_.bitrateKbps) + "kbps)");
    }

    // 入力形式(16bitPCM)
    ComPtr<IMFMediaType> inputType;
    hr = MFCreateMediaType(&inputType);
    if (SUCCEEDED(hr)) hr = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    if (SUCCEEDED(hr)) hr = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    if (SUCCEEDED(hr)) hr = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, format.nChannels);
    if (SUCCEEDED(hr)) hr = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, format.nSamplesPerSec);
    if (SUCCEEDED(hr)) hr = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, format.wBitsPerSample);
    if (SUCCEEDED(hr)) hr = inputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, format.nBlockAlign);
    if (SUCCEEDED(hr)) hr = inputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, format.nAvgBytesPerSec);
    if (SUCCEEDED(hr)) hr = inputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    if (SUCCEEDED(hr)) hr = writer->SetInputMediaType(streamIndex, inputType.Get(), nullptr);
    if (SUCCEEDED(hr)) hr = writer->BeginWriting();
    if (FAILED(hr)) {
        return Fail("mp3エンコーダーの初期化に失敗しました");
    }

    // 一定サイズごとにサンプルとして書き込む
    const size_t framesPerChunk = 4096;
    const size_t chunkBytes     = framesPerChunk * format.nBlockAlign;
    const double hnsPerByte     = 10000000.0 / format.nAvgBytesPerSec; //< 100ナノ秒単位

    for (size_t offset = 0; offset < bytes.size() && SUCCEEDED(hr); offset += chunkBytes) {
        const DWORD size = static_cast<DWORD>((std::min)(chunkBytes, bytes.size() - offset));

        ComPtr<IMFMediaBuffer> buffer;
        hr = MFCreateMemoryBuffer(size, &buffer);
        BYTE* dst = nullptr;
        if (SUCCEEDED(hr)) hr = buffer->Lock(&dst, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            std::memcpy(dst, bytes.data() + offset, size);
            hr = buffer->Unlock();
        }
        if (SUCCEEDED(hr)) hr = buffer->SetCurrentLength(size);

        ComPtr<IMFSample> sample;
        if (SUCCEEDED(hr)) hr = MFCreateSample(&sample);
        if (SUCCEEDED(hr)) hr = sample->AddBuffer(buffer.Get());
        if (SUCCEEDED(hr)) hr = sample->SetSampleTime(static_cast<LONGLONG>(offset * hnsPerByte));
        if (SUCCEEDED(hr)) hr = sample->SetSampleDuration(static_cast<LONGLONG>(size * hnsPerByte));
        if (SUCCEEDED(hr)) hr = writer->WriteSample(streamIndex, sample.Get());
    }
    if (FAILED(hr)) {
        return Fail("mp3の書き込み中にエラーが発生しました");
    }

    if (FAILED(writer->Finalize())) {
        return Fail("mp3の書き込み完了処理に失敗しました");
    }
    return true;
}

bool MP3Encoder::Fail(const std::string& message) {
    lastError_ = message;
    return false;
}
