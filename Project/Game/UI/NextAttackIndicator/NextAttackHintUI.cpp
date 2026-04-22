#include "NextAttackHintUI.h"

#include "3d/ViewProjection.h"
#include "Matrix4x4.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackAction.h"
#include "Player/Player.h"
#include "input/InputData.h"

#include <imgui.h>

void NextAttackHintUI::Init(Player* player) {
    pPlayer_ = player;

    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // LayoutParam は位置・矢印設定不要（毎フレーム SetPosition で直接指定）
    LayoutParam layout = {};

    xButtonUI_ = std::make_unique<ComboAsistButtonUI>();
    xButtonUI_->Init(XINPUT_GAMEPAD_X, false, layout, "");
    xButtonUI_->SetUnlockSoundEnabled(false);
    xButtonUI_->SetScale(spriteScale_);
    xButtonUI_->SetVisible(false);

    yButtonUI_ = std::make_unique<ComboAsistButtonUI>();
    yButtonUI_->Init(XINPUT_GAMEPAD_Y, false, layout, "");
    yButtonUI_->SetUnlockSoundEnabled(false);
    yButtonUI_->SetScale(spriteScale_);
    yButtonUI_->SetVisible(false);
}

void NextAttackHintUI::Update() {
    const ComboAttackAction::NextAttackCandidate* xCand = nullptr;
    const ComboAttackAction::NextAttackCandidate* yCand = nullptr;

    if (pPlayer_ && pViewProjection_) {
        auto* action = dynamic_cast<ComboAttackAction*>(pPlayer_->GetComboBehavior());
        if (action) {
            for (const auto& cand : action->GetAllNextAttackCandidates()) {
                if (!cand.branch) {
                    continue;
                }
                const int32_t btn = cand.branch->GetGamepadButton();
                if (btn == XINPUT_GAMEPAD_X && !xCand) {
                    xCand = &cand;
                }
                if (btn == XINPUT_GAMEPAD_Y && !yCand) {
                    yCand = &cand;
                }
            }
        }
    }

    // 候補がある場合のみ位置・ロック状態を更新
    if ((xCand || yCand) && pPlayer_ && pViewProjection_) {
        const Vector3 worldPos  = pPlayer_->GetWorldPosition() + worldOffset_;
        const Vector2 screenPos = ScreenTransform(worldPos, *pViewProjection_);

        if (xCand) {
            xButtonUI_->SetCurrentPosition(screenPos + xLocalOffset_);
            // HintUI では開放演出不要のため毎フレーム SnapUnlocked で同期
            xButtonUI_->SnapUnlocked(xCand->attackData->GetAttackParam().isUnlocked);
            xButtonUI_->SetLockUIScale(lockScale_);
        }

        if (yCand) {
            yButtonUI_->SetCurrentPosition(screenPos + yLocalOffset_);
            yButtonUI_->SnapUnlocked(yCand->attackData->GetAttackParam().isUnlocked);
            yButtonUI_->SetLockUIScale(lockScale_);
        }
    }

    // ScaleIn/ScaleOut イージングはトランジション時のみ発火（SetRangeVisible が管理）
    xButtonUI_->SetRangeVisible(xCand != nullptr);
    yButtonUI_->SetRangeVisible(yCand != nullptr);

    xButtonUI_->Update();
    yButtonUI_->Update();
}

void NextAttackHintUI::RegisterParams() {
    globalParameter_->Regist(groupName_, "worldOffset",  &worldOffset_);
    globalParameter_->Regist(groupName_, "spriteScale",  &spriteScale_);
    globalParameter_->Regist(groupName_, "xLocalOffset", &xLocalOffset_);
    globalParameter_->Regist(groupName_, "yLocalOffset", &yLocalOffset_);
    globalParameter_->Regist(groupName_, "lockScale",    &lockScale_);
}

void NextAttackHintUI::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat3("ワールドオフセット",  &worldOffset_.x,  0.05f);

        bool scaleChanged = ImGui::DragFloat2("スプライトスケール", &spriteScale_.x, 1.0f, 1.0f, 512.0f);
        if (scaleChanged) {
            xButtonUI_->SetScale(spriteScale_);
            yButtonUI_->SetScale(spriteScale_);
        }

        ImGui::DragFloat2("X ローカルオフセット", &xLocalOffset_.x, 1.0f);
        ImGui::DragFloat2("Y ローカルオフセット", &yLocalOffset_.x, 1.0f);
        ImGui::DragFloat2("ロック UI スケール",   &lockScale_.x,    1.0f, 1.0f, 512.0f);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
