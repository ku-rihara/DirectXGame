#include "OscillatorPanel.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include "Editor/AudioDSPEditor/AudioDSPContext.h"

OscillatorPanel::OscillatorPanel() {
    auto& p     = oscillator_.GetParam();
    p.waveform  = OscillatorSource::Waveform::SineSaw;
    p.frequency = 220.0;
}

void OscillatorPanel::Draw(AudioDSPContext& context) {
    const AudioDSPGui& gui = context.GetGui();
    auto& p                = oscillator_.GetParam();

    gui.EnumCombo("Waveform", p.waveform, [this](OscillatorSource::Waveform w) { return oscillator_.GetWaveformName(w); });
    gui.DragDouble("Frequency[Hz]", p.frequency, 1.0f, 20.0, 8000.0, "%.0f");
    gui.DragDouble("Amplitude", p.amplitude, 0.01f, 0.0, 1.0);
    gui.DragDouble("Duration[s]", duration_, 0.01f, 0.05, 10.0);

    if (ImGui::Button("生成")) {
        const size_t length = static_cast<size_t>(duration_ * sampleRate_);
        AudioPCM pcm(sampleRate_, channelCount_, length);
        pcm.FillAllChannels(oscillator_.Generate(length, sampleRate_));
        pcm.ApplyFade(fadeTime_, fadeTime_);
        context.SubmitResult(outputName_, pcm);
    }
    ImGui::SameLine();
    if (ImGui::Button("再生")) {
        context.Play(outputName_);
    }
}
