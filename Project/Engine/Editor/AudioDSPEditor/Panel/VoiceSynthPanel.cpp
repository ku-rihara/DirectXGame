#include "VoiceSynthPanel.h"

using namespace KetaEngine;
#include "Editor/AudioDSPEditor/AudioDSPContext.h"

void VoiceSynthPanel::Draw(AudioDSPContext& context) {
    const AudioDSPGui& gui = context.GetGui();
    auto& voice            = synthesizer_.GetParam();
    auto& glottal          = source_.GetParam();

    ImGui::InputText("Text(ローマ字)", text_, sizeof(text_));

    ImGui::SeparatorText("声帯(音源)");
    gui.DragDouble("Pitch[Hz]", glottal.pitch, 1.0f, 50.0, 800.0, "%.0f");
    gui.DragDouble("開大期の割合", glottal.openRatio, 0.01f, 0.05, 0.9);
    gui.DragDouble("閉小期の割合", glottal.closeRatio, 0.01f, 0.01, 0.5);

    ImGui::SeparatorText("声道(フォルマント)");
    gui.DragDouble("1音の長さ[s]", voice.moraDuration, 0.01f, 0.05, 1.0);
    gui.DragDouble("帯域幅[Hz]", voice.formantBandwidth, 1.0f, 10.0, 500.0, "%.0f");
    gui.DragDouble("母音フォルマント倍率", voice.vowelFormantScale, 0.01f, 0.5, 2.0);
    gui.DragDouble("子音フォルマント倍率", voice.consonantFormantScale, 0.01f, 0.5, 2.0);
    gui.DragDouble("子音の強さ", voice.consonantGain, 0.01f, 0.0, 2.0);
    gui.DragDouble("ディエンファシス", voice.deEmphasis, 0.001f, 0.0, 0.999, "%.3f");

    if (ImGui::Button("合成")) {
        context.SubmitResult(outputName_, synthesizer_.Synthesize(text_, source_));
    }
    ImGui::SameLine();
    if (ImGui::Button("再生")) {
        context.Play(outputName_);
    }
}
