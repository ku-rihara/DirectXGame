#include "AudienceController.h"
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
        audiences_[i]->SetBaseScale(baseTransformScale_);
        AdaptPositions(i);

        audiences_[i]->ForceUpdateTransforms();
    }
    createdCount_ = audienceMaxNum_;
}

void AudienceController::AdaptPositions(int32_t index) {

    if (audiences_[index]) {
        int32_t rowNum = audiences_[index]->GetSeatRowNum();
        audiences_[index]->AdaptPosition(GetSeatsRowZYPos(rowNum));
    }
}

void AudienceController::Update() {
    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        if (audiences_[i]) {
            if (isDebugForceShowAll_) {
                audiences_[i]->AppearByComboLevel(audiences_[i]->GetAppearComboLevel());
            }
            audiences_[i]->Update();
        }
    }
}

void AudienceController::AppearAudienceByLevel(int32_t level) {
    if (isDebugForceShowAll_) {
        return;
    }
    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        if (audiences_[i]) {
            audiences_[i]->AppearByComboLevel(level);
        }
    }
}

void AudienceController::DisAppearAudience(int32_t level) {
    for (int32_t i = 0; i < audienceMaxNum_; i++) {
        if (audiences_[i]) {
            audiences_[i]->DisAppearByComboLevel(level);
        }
    }
}

void AudienceController::RegisterParams() {
    globalParameter_->Regist<Vector2>(groupName_, "BaseSeatsZYPos", &baseSeatsZYPos_);
    globalParameter_->Regist<Vector2>(groupName_, "SeatsRowZYOffset", &seatsRowZYOffset_);
    globalParameter_->Regist<Vector3>(groupName_, "baseTransformScale", &baseTransformScale_);
}

Vector2 AudienceController::GetSeatsRowZYPos(int32_t row) const {
    // ベース位置 + オフセット × 列番号
    return Vector2{
        baseSeatsZYPos_.x + seatsRowZYOffset_.x * row,
        baseSeatsZYPos_.y + seatsRowZYOffset_.y * row};
}

void AudienceController::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::Checkbox("Debug Force Show All", &isDebugForceShowAll_);

        // transform
        ImGui::DragFloat3("baseScale", &baseTransformScale_.x, 0.01f);
        ImGui::DragFloat2("Base Seats ZY Pos", &baseSeatsZYPos_.x, 0.1f);

        // オフセット
        ImGui::DragFloat2("Seats Row ZY Offset", &seatsRowZYOffset_.x, 0.1f);

        for (int32_t i = 0; i < createdCount_; i++) {
            if (audiences_[i]) {
                audiences_[i]->SetBaseScale(baseTransformScale_);
                AdaptPositions(i);
            }
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Audiences")) {
        for (int32_t i = 0; i < createdCount_; i++) {
            if (audiences_[i]) {
                audiences_[i]->AdjustParam();
            }
        }
    }

#endif // _DEBUG
}
