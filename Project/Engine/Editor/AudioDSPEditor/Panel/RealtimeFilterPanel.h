#pragma once

#include "Audio/Audio.h"
#include "IAudioDSPPanel.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// XAudio2のリアルタイムフィルターを試すパネル
/// </summary>
class RealtimeFilterPanel : public IAudioDSPPanel {
public:
    explicit RealtimeFilterPanel(Audio& audio) : audio_(audio) {}

    const char* GetTitle() const override { return "リアルタイムフィルター (XAudio2)"; }
    void Draw(AudioDSPContext& context) override;

private:
    bool DrawVoiceFilter(const char* id, Audio::VoiceFilter& filter);

private:
    Audio& audio_;
    std::string soundName_;
    Audio::VoiceFilter voiceFilter_;
    Audio::VoiceFilter globalFilter_;
    bool isGlobalFilterEnabled_ = false;

    const char* filterTypeNames_[6] = {"LowPass", "BandPass", "HighPass", "Notch", "LowPassOnePole", "HighPassOnePole"};
};

}; // KetaEngine
