#pragma once

#include "Audio/Audio.h"
#include "Audio/DSP/AudioPCM.h"
#include "Audio/DSP/FIRFilter.h"
#include "Audio/DSP/IIRFilter.h"
#include "Audio/DSP/Vocoder.h"
#include "Audio/DSP/VoiceSynthesizer.h"
#include "Audio/DSP/WaveGenerator.h"
#include <string>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 波形加工(音声合成・フィルター・ボコーダ)のデバッグエディター
/// </summary>
class AudioDSPEditor {
public:
    AudioDSPEditor()  = default;
    ~AudioDSPEditor() = default;

    void Init();
    void EditorUpdate();

private:
    // 各セクション
    void DrawVoiceSynthesizer();
    void DrawWaveGenerator();
    void DrawOfflineFilter();
    void DrawVocoder();
    void DrawRealtimeFilter();
    void DrawPreview();

    // ヘルパー
    bool SoundCombo(const char* label, std::string& selected);
    void RegisterResult(const std::string& soundName, const AudioPCM& pcm);
    void SetPreview(const std::string& soundName);
    void SaveWav(const std::string& soundName);

private:
    enum class FilterKind {
        FIR,
        IIR,
    };

    Audio* audio_ = nullptr;

    // 音声合成
    VoiceSynthesizer synthesizer_;
    char synthText_[128] = "aiueo";

    // 波形生成
    WaveGenerator::Param generatorParam_;

    // オフラインフィルター
    std::string filterSource_;
    FilterKind filterKind_ = FilterKind::IIR;
    FIRFilter::Param firParam_;
    IIRFilter::Param iirParam_;

    // ボコーダ
    Vocoder vocoder_;
    std::string vocoderCarrier_;
    std::string vocoderModulator_;

    // リアルタイムフィルター
    std::string realtimeSource_;
    Audio::VoiceFilter voiceFilter_;
    Audio::VoiceFilter globalFilter_;
    bool isGlobalFilterEnabled_ = false;
    float playVolume_           = 0.7f;

    // プレビュー
    std::string previewName_;
    std::vector<float> previewWave_;
    std::vector<float> previewSpectrum_;
    double previewDuration_ = 0.0;
    int previewSampleRate_  = 0;
    std::string statusMessage_;

    // 生成したサウンドの名前
    const std::string voiceSoundName_     = "DSP_Voice.wav";
    const std::string generatorSoundName_ = "DSP_Wave.wav";
    const std::string filterSoundName_    = "DSP_Filtered.wav";
    const std::string vocoderSoundName_   = "DSP_Vocoder.wav";
    const std::string saveDirectory_      = "Resources/Audio/Generated/";
};

}; // KetaEngine
