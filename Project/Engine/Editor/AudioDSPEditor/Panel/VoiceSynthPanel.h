#pragma once

#include "Audio/DSP/Source/GlottalSource.h"
#include "Audio/DSP/Voice/VoiceSynthesizer.h"
#include "IAudioDSPPanel.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// 音声合成パネル
/// </summary>
class VoiceSynthPanel : public IAudioDSPPanel {
public:
    const char* GetTitle() const override { return "音声合成 (Formant)"; }
    void Draw(AudioDSPContext& context) override;

private:
    VoiceSynthesizer synthesizer_;
    GlottalSource source_;
    char text_[128] = "aiueo";

    const std::string outputName_ = "DSP_Voice.mp3";
};

}; // KetaEngine
