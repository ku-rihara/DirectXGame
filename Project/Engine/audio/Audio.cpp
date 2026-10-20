#include "Audio.h"

using namespace KetaEngine;
#include "DSP/AudioFilter.h"
#include "DSP/AudioPCM.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <numbers>
#define ATTENUATION_TIME_MS 50
#define ATTENUATION_FACTOR 0.01f 
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

Audio* Audio::GetInstance() {
    static Audio instance;
    return &instance;
}

void Audio::Init() {
    // XAudioエンジンのインスタンスを生成
    HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(hr));

    // マスターボイスを作成
    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
    assert(SUCCEEDED(hr));

    // 全体フィルター用のサブミックスボイスを作成(全ソースボイスはここを経由してマスターへ)
    XAUDIO2_VOICE_DETAILS masterDetails{};
    masterVoice_->GetVoiceDetails(&masterDetails);
    hr = xAudio2_->CreateSubmixVoice(&filterSubmixVoice_, masterDetails.InputChannels, masterDetails.InputSampleRate, XAUDIO2_VOICE_USEFILTER);
    assert(SUCCEEDED(hr));
    ClearGlobalFilter();

    // Media Foundationの初期化
    hr = MFStartup(MF_VERSION);
    if (SUCCEEDED(hr)) {
        isMediaFoundationInitialized_ = true;
    }

}

void Audio::LoadAllFilesInDirectory() {
    if (!std::filesystem::exists(filePath_)) {
        return; // ディレクトリが存在しない場合は何もしない
    }

    for (const auto& entry : std::filesystem::directory_iterator(filePath_)) {
        if (entry.is_regular_file()) {
            std::string filename  = entry.path().filename().string();
            std::string extension = entry.path().extension().string();

            // .mp3 または .wav ファイルのみロード
            if (extension == ".mp3" || extension == ".wav") {
                // 既にロード済みでない場合のみロード
                if (soundIndexMap_.find(entry.path().string()) == soundIndexMap_.end()) {
                    Load(filename);
                }
            }
        }
    }
}

int Audio::Load(const std::string& filename) {
    std::string fullPath = filePath_ + filename;

    // ファイルがすでに読み込まれている場合
    auto it = soundIndexMap_.find(fullPath);
    if (it != soundIndexMap_.end()) {
        return it->second; // 既存のインデックスを返す
    }

    // 拡張子で判定
    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos);

        if (extension == ".mp3") {
            return LoadMP3File(fullPath);
        } else if (extension == ".wav") {
            return LoadWaveFile(fullPath);
        }
    }

    // デフォルトはWAVとして読み込み
    assert(0 && "未対応のファイル形式です");
    return -1;
}

int Audio::LoadWaveFile(const std::string& fullPath) {

    std::ifstream file;
    file.open(fullPath, std::ios_base::binary);
    assert(file.is_open());

    // RIFFヘッダーの読み込み
    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));
    if (strncmp(riff.chunk.id, "RIFF", 4) != 0 || strncmp(riff.type, "WAVE", 4) != 0) {
        assert(0);
    }

    // Formatチャンクの読み込み
    FormatChunk format = {};
    file.read((char*)&format, sizeof(ChunkHeader));
    assert(strncmp(format.chunk.id, "fmt ", 4) == 0);
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);

    // Dataチャンクの読み込み
    ChunkHeader chunk;
    std::vector<BYTE> buffer;

    while (file.read((char*)&chunk, sizeof(chunk))) {
        if (strncmp(chunk.id, "data", 4) == 0) {
            // 波形データの読み込み
            buffer.resize(chunk.size);
            file.read(reinterpret_cast<char*>(buffer.data()), chunk.size);
            break;
        } else {
            // その他チャンクをスキップ
            file.seekg(chunk.size, std::ios_base::cur);
        }
    }
    assert(!buffer.empty());
    file.close();

    // 読み込んだ音声データを保存
    SoundData soundData  = {};
    soundData.wfex       = format.fmt;
    soundData.bufferSize = int(buffer.size());
    soundData.buffer     = std::move(buffer);

    return RegisterSoundData(fullPath, std::move(soundData));
}

int Audio::LoadMP3File(const std::string& fullPath) {
    assert(isMediaFoundationInitialized_);

    HRESULT hr;

    // ファイルパスをワイド文字列に変換
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, NULL, 0);
    std::vector<wchar_t> wstr(wchars_num);
    MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, wstr.data(), wchars_num);

    // SourceReaderの作成
    Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
    hr = MFCreateSourceReaderFromURL(wstr.data(), NULL, &pReader);
    assert(SUCCEEDED(hr));

    // PCMフォーマットを設定
    Microsoft::WRL::ComPtr<IMFMediaType> pMediaType;
    hr = MFCreateMediaType(&pMediaType);
    assert(SUCCEEDED(hr));

    hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    assert(SUCCEEDED(hr));
    hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    assert(SUCCEEDED(hr));

    hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pMediaType.Get());
    assert(SUCCEEDED(hr));

    // 実際のフォーマットを取得
    Microsoft::WRL::ComPtr<IMFMediaType> pOutputType;
    hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);
    assert(SUCCEEDED(hr));

    // WAVEFORMATEXに変換
    WAVEFORMATEX* pWaveFormat = NULL;
    UINT32 cbFormat           = 0;
    hr                        = MFCreateWaveFormatExFromMFMediaType(pOutputType.Get(), &pWaveFormat, &cbFormat);
    assert(SUCCEEDED(hr));

    WAVEFORMATEX wfex = *pWaveFormat;
    CoTaskMemFree(pWaveFormat);

    // オーディオデータを読み込み
    std::vector<BYTE> audioData;

    while (true) {
        Microsoft::WRL::ComPtr<IMFSample> pSample;
        DWORD streamFlags = 0;

        hr = pReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            NULL,
            &streamFlags,
            NULL,
            &pSample);

        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
            break;
        }

        if (pSample == nullptr) {
            continue;
        }

        Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
        assert(SUCCEEDED(hr));

        BYTE* pAudioData      = NULL;
        DWORD cbCurrentLength = 0;
        hr                    = pBuffer->Lock(&pAudioData, NULL, &cbCurrentLength);
        assert(SUCCEEDED(hr));

        audioData.insert(audioData.end(), pAudioData, pAudioData + cbCurrentLength);

        hr = pBuffer->Unlock();
    }

    // 読み込んだ音声データを保存
    SoundData soundData  = {};
    soundData.wfex       = wfex;
    soundData.bufferSize = int(audioData.size());
    soundData.buffer     = std::move(audioData);

    return RegisterSoundData(fullPath, std::move(soundData));
}

int Audio::RegisterSoundData(const std::string& fullPath, SoundData&& soundData) {
    // soundDatas_に追加
    // 同名の差し替え時も、再生中のボイスが古いバッファを参照している可能性があるため上書きせず新規に追加する
    soundDatas_.push_back(std::move(soundData));
    int index = int(soundDatas_.size()) - 1;

    // ファイル名とインデックスをマップに保存
    soundIndexMap_[fullPath] = index;

    return index;
}

void Audio::Play(const std::string& filename, float volume) {

    // ファイルパスを生成
    std::string fullPath = filePath_ + filename;

    // マップから既存のサウンドIDを検索
    auto it = soundIndexMap_.find(fullPath);

    int soundId;
    if (it != soundIndexMap_.end()) {
        // 既に読み込まれている場合はそのIDを使用
        soundId = it->second;
    } else {
        // 読み込まれていない場合は新規に読み込み
        soundId = Load(filename);
        if (soundId == -1) {
            // 読み込みに失敗した場合は何もしない
            return;
        }
    }

    // IDで再生
    PlayForID(soundId, volume);
}

void Audio::PlayForID(int soundId, float volume, const VoiceFilter* filter) {
    if (soundId < 0 || soundId >= soundDatas_.size()) {
        return;
    }

    const SoundData& soundData = soundDatas_[soundId];
    if (soundData.buffer.empty()) {
        return;
    }

    HRESULT result;
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    // 出力先を全体フィルター用のサブミックスに設定
    XAUDIO2_SEND_DESCRIPTOR sendDescriptor = {0, filterSubmixVoice_};
    XAUDIO2_VOICE_SENDS sendList           = {1, &sendDescriptor};
    const UINT32 voiceFlags                = filter ? XAUDIO2_VOICE_USEFILTER : 0;

    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex, voiceFlags, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr,
        filterSubmixVoice_ ? &sendList : nullptr);
    assert(SUCCEEDED(result));

    // 個別フィルター
    if (filter) {
        XAUDIO2_FILTER_PARAMETERS params = ToFilterParameters(*filter, soundData.wfex.nSamplesPerSec);
        result                           = pSourceVoice->SetFilterParameters(&params);
        assert(SUCCEEDED(result));
    }

    // ボリューム調整
    float adjustedVolume = volume;
    auto now             = std::chrono::steady_clock::now();

    if (lastPlayTimes_.find(soundId) != lastPlayTimes_.end()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPlayTimes_[soundId]).count();
        if (elapsed < ATTENUATION_TIME_MS) {
            // 直前に同じ音が鳴った場合、ボリュームを下げる
            float factor = 1.0f - (1.0f - ATTENUATION_FACTOR) * (1.0f - float(elapsed) / ATTENUATION_TIME_MS);
            adjustedVolume *= factor; // 重なり具合に応じてボリュームを調整
        }
    }
    lastPlayTimes_[soundId] = now; // 最新の再生時間を記録

    result = pSourceVoice->SetVolume(adjustedVolume);
    assert(SUCCEEDED(result));

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.buffer.data();
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags      = XAUDIO2_END_OF_STREAM;

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));

    result = pSourceVoice->Start();
    assert(SUCCEEDED(result));
}

std::vector<std::string> Audio::GetAvailableSoundNames() const {
    std::vector<std::string> names;
    for (const auto& pair : soundIndexMap_) {
        // フルパスからファイル名（拡張子付き）を抽出
        std::filesystem::path p(pair.first);
        names.push_back(p.filename().string());
    }
    return names;
}

void Audio::Unload(int soundId) {
    if (soundId >= 0 && soundId < soundDatas_.size()) {
        SoundData& soundData = soundDatas_[soundId];
        soundData.buffer.clear();
        soundData.buffer.shrink_to_fit();
        soundData.bufferSize = 0;
        soundData.wfex       = {};
    }
}

void Audio::PlayWithFilter(const std::string& soundName, const VoiceFilter& filter, float volume) {
    int soundId = FindSoundId(soundName);
    if (soundId == -1) {
        soundId = Load(soundName);
    }
    PlayForID(soundId, volume, &filter);
}

void Audio::SetGlobalFilter(const VoiceFilter& filter) {
    if (!filterSubmixVoice_) {
        return;
    }
    XAUDIO2_VOICE_DETAILS details{};
    filterSubmixVoice_->GetVoiceDetails(&details);

    XAUDIO2_FILTER_PARAMETERS params = ToFilterParameters(filter, details.InputSampleRate);
    filterSubmixVoice_->SetFilterParameters(&params);

    globalFilter_          = filter;
    isGlobalFilterEnabled_ = true;
}

void Audio::ClearGlobalFilter() {
    if (!filterSubmixVoice_) {
        return;
    }
    // 既定値(LowPass, Frequency=1.0, OneOverQ=1.0)はフィルター無しと同等
    XAUDIO2_FILTER_PARAMETERS params = {LowPassFilter, XAUDIO2_DEFAULT_FILTER_FREQUENCY, XAUDIO2_DEFAULT_FILTER_ONEOVERQ};
    filterSubmixVoice_->SetFilterParameters(&params);

    isGlobalFilterEnabled_ = false;
}

XAUDIO2_FILTER_PARAMETERS Audio::ToFilterParameters(const VoiceFilter& filter, UINT32 sampleRate) {
    XAUDIO2_FILTER_PARAMETERS params{};
    params.Type = filter.type;

    // 周波数[Hz]をXAudio2の係数に変換(OnePole系は変換式が異なる)
    // (xaudio2.hのXAudio2CutoffFrequencyToRadians等と同じ式。あちらはXAUDIO2_HELPER_FUNCTIONS定義時のみ有効)
    const float frequency = (std::max)(filter.frequency, 1.0f);
    const float rate      = static_cast<float>(sampleRate);
    if (filter.type == LowPassOnePoleFilter || filter.type == HighPassOnePoleFilter) {
        const float ratio = 1.0f - 2.0f * frequency / rate;
        params.Frequency  = (frequency >= rate) ? XAUDIO2_MAX_FILTER_FREQUENCY : 1.0f - ratio * ratio;
    } else {
        // 状態変数フィルターはサンプリング周波数の1/6までしか扱えない
        params.Frequency = (frequency * 6.0f >= rate) ? XAUDIO2_MAX_FILTER_FREQUENCY
                                                      : 2.0f * std::sin(std::numbers::pi_v<float> * frequency / rate);
    }

    // 1/Qは0より大きくXAUDIO2_MAX_FILTER_ONEOVERQ以下
    const float q    = (std::max)(filter.q, 1.0f / XAUDIO2_MAX_FILTER_ONEOVERQ);
    params.OneOverQ  = (std::min)(1.0f / q, XAUDIO2_MAX_FILTER_ONEOVERQ);
    return params;
}

int Audio::FindSoundId(const std::string& soundName) const {
    auto it = soundIndexMap_.find(filePath_ + soundName);
    return (it != soundIndexMap_.end()) ? it->second : -1;
}

bool Audio::GetPCM(const std::string& soundName, AudioPCM& outPCM) const {
    const int soundId = FindSoundId(soundName);
    if (soundId < 0) {
        return false;
    }
    const SoundData& soundData = soundDatas_[soundId];
    return outPCM.FromBytes(soundData.wfex, soundData.buffer);
}

int Audio::RegisterPCM(const std::string& soundName, const AudioPCM& pcm) {
    if (pcm.IsEmpty()) {
        return -1;
    }
    SoundData soundData{};
    pcm.ToBytes16(soundData.wfex, soundData.buffer);
    soundData.bufferSize = static_cast<unsigned int>(soundData.buffer.size());

    return RegisterSoundData(filePath_ + soundName, std::move(soundData));
}

int Audio::CreateFilteredSound(const std::string& srcName, const std::string& dstName, AudioFilter& filter) {
    AudioPCM pcm;
    if (!GetPCM(srcName, pcm)) {
        return -1;
    }
    filter.Process(pcm);
    return RegisterPCM(dstName, pcm);
}

void Audio::Finalize() {
    if (filterSubmixVoice_) {
        filterSubmixVoice_->DestroyVoice();
        filterSubmixVoice_ = nullptr;
    }
    if (masterVoice_) {
        masterVoice_->DestroyVoice();
        masterVoice_ = nullptr;
    }
    xAudio2_.Reset();

    soundDatas_.clear();

    if (isMediaFoundationInitialized_) {
        MFShutdown();
        isMediaFoundationInitialized_ = false;
    }
}