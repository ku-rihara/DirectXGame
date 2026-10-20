#include "FilterPanel.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include "Audio/DSP/IO/ISoundRegistry.h"
#include "Editor/AudioDSPEditor/AudioDSPContext.h"

void FilterPanel::Draw(AudioDSPContext& context) {
    context.GetGui().SoundCombo("Source", sourceName_, context.GetRegistry());

    int kind = static_cast<int>(kind_);
    ImGui::RadioButton("IIR", &kind, static_cast<int>(Kind::IIR));
    ImGui::SameLine();
    ImGui::RadioButton("FIR", &kind, static_cast<int>(Kind::FIR));
    kind_ = static_cast<Kind>(kind);

    if (kind_ == Kind::IIR) {
        DrawIIRParam(context);
    } else {
        DrawFIRParam(context);
    }

    if (ImGui::Button("フィルター適用")) {
        Apply(context);
    }
    ImGui::SameLine();
    if (ImGui::Button("再生(元)")) {
        context.Play(sourceName_);
    }
    ImGui::SameLine();
    if (ImGui::Button("再生(結果)")) {
        context.Play(outputName_);
    }
}

void FilterPanel::DrawIIRParam(AudioDSPContext& context) {
    const AudioDSPGui& gui = context.GetGui();
    auto& p                = iirParam_;

    gui.EnumCombo("Type", p.type, [this](IIRFilter::Type t) { return iirFilter_.GetTypeName(t); });
    const bool isBand = p.type == IIRFilter::Type::BandPass || p.type == IIRFilter::Type::BandEliminate;
    gui.DragDouble(isBand ? "Low[Hz]" : "Frequency[Hz]", p.frequency, 10.0f, 20.0, 20000.0, "%.0f");
    if (isBand) {
        gui.DragDouble("High[Hz]", p.frequency2, 10.0f, 20.0, 20000.0, "%.0f");
    } else {
        gui.DragDouble("Q", p.q, 0.01f, 0.1, 20.0);
    }
}

void FilterPanel::DrawFIRParam(AudioDSPContext& context) {
    const AudioDSPGui& gui = context.GetGui();
    auto& p                = firParam_;

    gui.EnumCombo("Type", p.type, [this](FIRFilter::Type t) { return firFilter_.GetTypeName(t); });
    const bool isBand = p.type == FIRFilter::Type::BandPass || p.type == FIRFilter::Type::BandEliminate;
    gui.DragDouble(isBand ? "Low[Hz]" : "Frequency[Hz]", p.frequency, 10.0f, 20.0, 20000.0, "%.0f");
    if (isBand) {
        gui.DragDouble("High[Hz]", p.frequency2, 10.0f, 20.0, 20000.0, "%.0f");
    }
    ImGui::DragInt("遅延器の数J", &p.delayCount, 2.0f, 2, 1024);
    gui.EnumCombo("Window", p.window, [this](WindowFunction::Type t) { return window_.GetTypeName(t); });
}

void FilterPanel::Apply(AudioDSPContext& context) {
    AudioPCM pcm;
    if (sourceName_.empty() || !context.GetRegistry().TryGet(sourceName_, pcm)) {
        context.SetStatus("元のサウンドを取得できません: " + sourceName_);
        return;
    }

    if (kind_ == Kind::IIR) {
        iirFilter_.Setup(iirParam_, pcm.GetSampleRate());
        processor_.Apply(iirFilter_, pcm);
    } else {
        firFilter_.Setup(firParam_, pcm.GetSampleRate());
        processor_.Apply(firFilter_, pcm);
    }
    context.SubmitResult(outputName_, pcm);
}
