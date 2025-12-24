#include "AudienceController.h"
#include "Frame/Frame.h"
#include <imGui.h>

void AudienceController::Init() {

    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        audiences_[i] = std::make_unique<Audience>();
        audiences_[i]->Init(i);
        AdaptPositions(i);
    }
}

void AudienceController::AdaptPositions(int32_t index) {

    if (audiences_[index]) {
        SeatsRow seatsRow = audiences_[index]->GetSeatsRow();
        audiences_[index]->AdaptPosition(seatsRowZYPos_[static_cast<int32_t>(seatsRow)]);
    }
}

void AudienceController::Update() {
    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        audiences_[i]->Update();
    }
}

void AudienceController::AppearAudienceByLevel(int32_t level) {
    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        audiences_[i]->AppearByComboLevel(level);
    }
}

void AudienceController::RegisterParams() {

    for (int32_t i = 0; i < seatsRowZYPos_.size(); i++) {
        std::string key = "SeatsRowZYPos" + SeatsRowToString(static_cast<SeatsRow>(i));
        globalParameter_->Regist<Vector2>(groupName_, key, &seatsRowZYPos_[i]);
    }
}

void AudienceController::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (int32_t i = 0; i < seatsRowZYPos_.size(); i++) {
            ImGui::DragFloat2(
                ("SeatsRowZYPos_" + SeatsRowToString(static_cast<SeatsRow>(i))).c_str(),
                &seatsRowZYPos_[i].x,
                0.1f);
        }

        for (int32_t i = 0; i < audienceMaxNum_; i++) {
            AdaptPositions(i);
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Audiences")) {
        for (int32_t i = 0; i < audienceMaxNum_; i++) {
            audiences_[i]->AdjustParam();
        }
    }

#endif // _DEBUG
}

std::string AudienceController::SeatsRowToString(SeatsRow seatsRow) {

    switch (seatsRow) {
    case SeatsRow::FRONT:
        return "Front";
    case SeatsRow::MIDDLE:
        return "Middle";
    case SeatsRow::BACK:
        return "Back";
    case SeatsRow::COUNT:
        return "Count";
    }

    return "Unknown";
}