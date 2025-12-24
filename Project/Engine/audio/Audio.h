#pragma once

#include <chrono>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include <xaudio2.h>

/// <summary>
///  オーディオクラス
/// </summary>
namespace KetaEngine {

class Audio {
public:
    struct ChunkHeader {
        char id[4]; // チャンク毎のID
        int32_t size; // チャンクサイズ
    };
    // RIFFヘッダチャンク
    struct RiffHeader {
        ChunkHeader chunk; // RIFF
        char type[4];
    };
    // FNTチャンク
    struct FormatChunk {
        ChunkHeader chunk; //"fmt"
        WAVEFORMATEX fmt; // 波形フォーマット
    };

    struct SoundData {
        // 波形フォーマット
        WAVEFORMATEX wfex;
        // バッファの先頭アドレス
        BYTE* pBuffer;
        // バッファのサイズ
        unsigned int bufferSize;
    };

private:
    // XAudio2のインスタンス
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masterVoice_;
    std::vector<SoundData> soundDatas_;
    std::unordered_map<std::string, int> soundIndexMap_;

    std::unordered_map<int, std::chrono::steady_clock::time_point> lastPlayTimes_;

    const std::string filePath_ = "Resources/Audio/";

    bool isMediaFoundationInitialized_ = false;

    // 内部ヘルパー関数
    int LoadWaveFile(const std::string& fullPath);
    int LoadMP3File(const std::string& fullPath);
    void PlayForID(const int& soundId, float volume);

public:
    static Audio* GetInstance();

    Audio()  = default;
    ~Audio() = default;

    void Init();
    int Load(const std::string& filename);
    void Unload(const int& soundId);
    void Play(const std::string& soundName, float volume);
    void Finalize();

    IXAudio2* GetXAudio2() const { return xAudio2_.Get(); }
};

}; // KetaEngine