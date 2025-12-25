#include "AudienceController.h"
#include "Frame/Frame.h"
#include <imGui.h>

void AudienceController::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        audiences_[i] = std::make_unique<Audience>();
        audiences_[i]->Init(i);
        AdaptPositions(i);
    }
}

void AudienceController::AdaptPositions(int32_t index) {

    if (audiences_[index]) {
        int32_t rowNum = audiences_[index]->GetSeatRowNum();
        audiences_[index]->AdaptPosition(GetSeatsRowZYPos(rowNum));
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
    globalParameter_->Regist<Vector2>(groupName_, "BaseSeatsZYPos", &baseSeatsZYPos_);
    globalParameter_->Regist<Vector2>(groupName_, "SeatsRowZYOffset", &seatsRowZYOffset_);
}

Vector2 AudienceController::GetSeatsRowZYPos(int32_t row) const {
    // ベース位置 + オフセット × 列番号
    return Vector2{
        baseSeatsZYPos_.x + seatsRowZYOffset_.x * row,
        baseSeatsZYPos_.y + seatsRowZYOffset_.y * row};
}

void AudienceController::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // ベース位置
        ImGui::DragFloat2("Base Seats ZY Pos", &baseSeatsZYPos_.x, 0.1f);

        // オフセット
        ImGui::DragFloat2("Seats Row ZY Offset", &seatsRowZYOffset_.x, 0.1f);

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
