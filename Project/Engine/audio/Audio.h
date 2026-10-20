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

namespace KetaEngine {

/// <summary>
///  オーディオクラス
/// </summary>
class Audio {
public:
    /// <summary>
    /// WAVEファイルのチャンク共通ヘッダ
    /// </summary>
    struct ChunkHeader {
        char id[4]; // チャンク毎のID
        int32_t size; // チャンクサイズ
    };
    /// <summary>
    /// RIFFヘッダチャンク
    /// </summary>
    struct RiffHeader {
        ChunkHeader chunk; // RIFF
        char type[4];
    };
    /// <summary>
    /// FNTチャンク
    /// </summary>
    struct FormatChunk {
        ChunkHeader chunk; //"fmt"
        WAVEFORMATEX fmt; // 波形フォーマット
    };

    /// <summary>
    /// 読み込み済み音声1件分のフォーマットとPCMバッファ
    /// </summary>
    struct SoundData {
        // 波形フォーマット
        WAVEFORMATEX wfex;
        // 波形データ
        std::vector<BYTE> buffer;
        // バッファのサイズ
        unsigned int bufferSize;
    };

    /// <summary>
    /// 再生時に掛けるリアルタイムフィルター(XAudio2のボイスフィルター)
    /// </summary>
    struct VoiceFilter {
        XAUDIO2_FILTER_TYPE type = LowPassFilter;
        float frequency          = 1000.0f; //< 遮断/中心周波数[Hz]
        float q                  = 1.0f;    //< Q値(OnePole系では無効)
    };

private:
    // XAudio2のインスタンス
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masterVoice_;
    IXAudio2SubmixVoice* filterSubmixVoice_ = nullptr; //< 全体フィルター用
    std::vector<SoundData> soundDatas_;
    std::unordered_map<std::string, int> soundIndexMap_;

    std::unordered_map<int, std::chrono::steady_clock::time_point> lastPlayTimes_;

    const std::string filePath_ = "Resources/Audio/";

    bool isMediaFoundationInitialized_ = false;

    // 全体フィルター
    bool isGlobalFilterEnabled_ = false;
    VoiceFilter globalFilter_;

    // 内部ヘルパー関数
    int LoadWaveFile(const std::string& fullPath);
    int LoadMP3File(const std::string& fullPath);
    int RegisterSoundData(const std::string& fullPath, SoundData&& soundData);
    void PlayForID(int soundId, float volume, const VoiceFilter* filter = nullptr);
    static XAUDIO2_FILTER_PARAMETERS ToFilterParameters(const VoiceFilter& filter, UINT32 sampleRate);

public:
    // 全ファイルロード用
    void LoadAllFilesInDirectory();

    static Audio* GetInstance();

private:
    Audio()                        = default;
    ~Audio()                       = default;
    // コピー禁止
    Audio(const Audio&)            = delete;
    Audio& operator=(const Audio&) = delete;

public:
    void Init();
    int Load(const std::string& filename);
    void Unload(int soundId);
    void Play(const std::string& soundName, float volume = 1.0f);
    void Finalize();

    ///------------------------------------------------------------
    /// フィルター
    ///------------------------------------------------------------

    // フィルターを掛けて再生
    void PlayWithFilter(const std::string& soundName, const VoiceFilter& filter, float volume = 1.0f);

    // 全サウンドに掛かるフィルターを設定/解除
    void SetGlobalFilter(const VoiceFilter& filter);
    void ClearGlobalFilter();

    // サウンド名からIDを取得(未ロードは-1)
    int FindSoundId(const std::string& soundName) const;

    IXAudio2* GetXAudio2() const { return xAudio2_.Get(); }
    bool IsGlobalFilterEnabled() const { return isGlobalFilterEnabled_; }
    const VoiceFilter& GetGlobalFilter() const { return globalFilter_; }

    // 読み込み済みサウンド名一覧
    std::vector<std::string> GetAvailableSoundNames() const;
};

}; // KetaEngine