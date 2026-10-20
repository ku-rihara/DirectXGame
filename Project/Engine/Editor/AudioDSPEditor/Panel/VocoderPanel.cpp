#include "VocoderPanel.h"

using namespace KetaEngine;
#include "Audio/DSP/IO/ISoundRegistry.h"
#include "Editor/AudioDSPEditor/AudioDSPContext.h"

void VocoderPanel::Draw(AudioDSPContext& context) {
    const AudioDSPGui& gui = context.GetGui();
    auto& p                = vocoder_.GetParam();

    gui.SoundCombo("Carrier(楽器)", carrierName_, context.GetRegistry());
    gui.SoundCombo("Modulator(声)", modulatorName_, context.GetRegistry());
    ImGui::DragInt("FFT Size", &p.fftSize, 1.0f, 64, 8192);
    ImGui::DragInt("Band Width[bin]", &p.bandWidth, 1.0f, 1, 64);
    gui.EnumCombo("Window", p.window, [this](WindowFunction::Type t) { return window_.GetTypeName(t); });

    if (ImGui::Button("ボコーダ適用")) {
        Apply(context);
    }
    ImGui::SameLine();
    if (ImGui::Button("再生")) {
        context.Play(outputName_);
    }
}

void VocoderPanel::Apply(AudioDSPContext& context) {
    AudioPCM carrier, modulator;
    const ISoundRegistry& registry = context.GetRegistry();
    if (!registry.TryGet(carrierName_, carrier) || !registry.TryGet(modulatorName_, modulator)) {
        context.SetStatus("Carrier/Modulatorを取得できません");
        return;
    }
    if (carrier.GetSampleRate() != modulator.GetSampleRate()) {
        context.SetStatus("Carrier/Modulatorのサンプリング周波数が異なります");
        return;
    }
    context.SubmitResult(outputName_, vocoder_.Process(carrier, modulator));
}
