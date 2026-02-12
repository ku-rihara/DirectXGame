#include "Audio.h"

using namespace KetaEngine;
#include <assert.h>
#include <cctype>
#include <filesystem>
#include <fstream>
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
    char* pBuffer   = nullptr;
    size_t dataSize = 0;

    while (file.read((char*)&chunk, sizeof(chunk))) {
        if (strncmp(chunk.id, "data", 4) == 0) {
            // 波形データの読み込み
            pBuffer = new char[chunk.size];
            file.read(pBuffer, chunk.size);
            dataSize = chunk.size;
            break;
        } else {
            // その他チャンクをスキップ
            file.seekg(chunk.size, std::ios_base::cur);
        }
    }
    assert(pBuffer != nullptr && dataSize > 0);
    file.close();

    // 読み込んだ音声データを保存
    SoundData soundData  = {};
    soundData.wfex       = format.fmt;
    soundData.pBuffer    = reinterpret_cast<BYTE*>(pBuffer);
    soundData.bufferSize = int(dataSize);

    // soundDatas_に追加
    soundDatas_.push_back(soundData);
    int index = int(soundDatas_.size()) - 1;

    // ファイル名とインデックスをマップに保存
    soundIndexMap_[fullPath] = index;

    return index;
}

int Audio::LoadMP3File(const std::string& fullPath) {
    assert(isMediaFoundationInitialized_);

    HRESULT hr;

    // ファイルパスをワイド文字列に変換
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, NULL, 0);
    wchar_t* wstr  = new wchar_t[wchars_num];
    MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, wstr, wchars_num);

    // SourceReaderの作成
    Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
    hr = MFCreateSourceReaderFromURL(wstr, NULL, &pReader);
    delete[] wstr;
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

    // バッファにコピー
    BYTE* pBuffer = new BYTE[audioData.size()];
    memcpy(pBuffer, audioData.data(), audioData.size());

    // 読み込んだ音声データを保存
    SoundData soundData  = {};
    soundData.wfex       = wfex;
    soundData.pBuffer    = pBuffer;
    soundData.bufferSize = int(audioData.size());

    // soundDatas_に追加
    soundDatas_.push_back(soundData);
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

void Audio::PlayForID(int soundId, float volume) {
    if (soundId < 0 || soundId >= soundDatas_.size()) {
        return;
    }

    const SoundData& soundData = soundDatas_[soundId];

    HRESULT result;
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

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
    buf.pAudioData = soundData.pBuffer;
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
        delete[] soundData.pBuffer;
        soundData.pBuffer    = nullptr;
        soundData.bufferSize = 0;
        soundData.wfex       = {};
    }
}

void Audio::Finalize() {
    for (auto& soundData : soundDatas_) {
        delete[] soundData.pBuffer;
        soundData.pBuffer = nullptr;
    }
    soundDatas_.clear();

    if (isMediaFoundationInitialized_) {
        MFShutdown();
        isMediaFoundationInitialized_ = false;
    }

    xAudio2_.Reset();
}