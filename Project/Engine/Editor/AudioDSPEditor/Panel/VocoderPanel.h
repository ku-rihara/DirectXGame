#pragma once

#include "Audio/DSP/Effect/Vocoder.h"
#include "IAudioDSPPanel.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// ボコーダパネル
/// </summary>
class VocoderPanel : public IAudioDSPPanel {
public:
    const char* GetTitle() const override { return "ボコーダ"; }
    void Draw(AudioDSPContext& context) override;

private:
    void Apply(AudioDSPContext& context);

private:
    Vocoder vocoder_;
    WindowFunction window_;
    std::string carrierName_;
    std::string modulatorName_;

    const std::string outputName_ = "DSP_Vocoder.mp3";
};

}; // KetaEngine
