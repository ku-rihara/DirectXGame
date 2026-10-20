#include "RealtimeFilterPanel.h"

using namespace KetaEngine;
#include "Editor/AudioDSPEditor/AudioDSPContext.h"

void RealtimeFilterPanel::Draw(AudioDSPContext& context) {
    ImGui::SeparatorText("個別 (PlayWithFilter)");
    context.GetGui().SoundCombo("Sound", soundName_, context.GetRegistry());
    DrawVoiceFilter("voice", voiceFilter_);
    if (ImGui::Button("フィルター付き再生") && !soundName_.empty()) {
        audio_.PlayWithFilter(soundName_, voiceFilter_, context.GetPlayVolume());
    }

    ImGui::SeparatorText("全体 (SetGlobalFilter)");
    bool changed = ImGui::Checkbox("有効", &isGlobalFilterEnabled_);
    changed |= DrawVoiceFilter("global", globalFilter_);
    if (changed) {
        if (isGlobalFilterEnabled_) {
            audio_.SetGlobalFilter(globalFilter_);
        } else {
            audio_.ClearGlobalFilter();
        }
    }
}

bool RealtimeFilterPanel::DrawVoiceFilter(const char* id, Audio::VoiceFilter& filter) {
    ImGui::PushID(id);
    int type     = static_cast<int>(filter.type);
    bool changed = ImGui::Combo("Type", &type, filterTypeNames_, IM_ARRAYSIZE(filterTypeNames_));
    filter.type  = static_cast<XAUDIO2_FILTER_TYPE>(type);
    changed |= ImGui::DragFloat("Frequency[Hz]", &filter.frequency, 10.0f, 20.0f, 20000.0f, "%.0f");
    changed |= ImGui::DragFloat("Q", &filter.q, 0.01f, 0.67f, 10.0f, "%.2f");
    ImGui::PopID();
    return changed;
}
