#include "NextAttackHintUI.h"

#include "3d/ViewProjection.h"
#include "Matrix4x4.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackAction.h"
#include "Player/Player.h"

#include <imgui.h>

// GamepadButton enum の X = 2, Y = 3 に対応
static constexpr int32_t kButtonX = 2;
static constexpr int32_t kButtonY = 3;

void NextAttackHintUI::Init(Player* player) {
    pPlayer_ = player;

    xSprite_.reset(KetaEngine::Sprite::Create("ButtomUI/OperateX.dds", false));
    ySprite_.reset(KetaEngine::Sprite::Create("ButtomUI/OperateY.dds", false));

    if (xSprite_) {
        xSprite_->SetAnchorPoint({0.5f, 0.5f});
        xSprite_->SetIsDraw(false);
    }
    if (ySprite_) {
        ySprite_->SetAnchorPoint({0.5f, 0.5f});
        ySprite_->SetIsDraw(false);
    }

    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void NextAttackHintUI::Update() {
    // 毎フレーム非表示にリセット
    if (xSprite_) {
        xSprite_->SetIsDraw(false);
    }
    if (ySprite_) {
        ySprite_->SetIsDraw(false);
    }

    if (!pPlayer_ || !pViewProjection_) {
        return;
    }

    // ComboAttackAction 中のみ表示
    auto* behavior = pPlayer_->GetComboBehavior();
    auto* action   = dynamic_cast<ComboAttackAction*>(behavior);
    if (!action) {
        return;
    }

    const auto& candidates = action->GetNextAttackCandidates();
    if (candidates.empty()) {
        return;
    }

    // X / Y ボタンの候補有無を判定
    bool hasX = false;
    bool hasY = false;
    for (const auto& cand : candidates) {
        if (!cand.branch) {
            continue;
        }
        const int32_t btn = cand.branch->GetGamepadButton();
        if (btn == kButtonX) {
            hasX = true;
        }
        if (btn == kButtonY) {
            hasY = true;
        }
    }

    if (!hasX && !hasY) {
        prevXVisible_ = false;
        prevYVisible_ = false;
        return;
    }

    // ワールド座標 → スクリーン座標変換
    const Vector3 worldPos  = pPlayer_->GetWorldPosition() + worldOffset_;
    const Vector2 screenPos = ScreenTransform(worldPos, *pViewProjection_);

   /* const bool both = hasX && hasY;*/

    if (hasX && xSprite_) {
        ApplySprite(xSprite_.get(), screenPos + xLocalOffset_, prevXVisible_);
    }

    if (hasY && ySprite_) {
        ApplySprite(ySprite_.get(), screenPos + yLocalOffset_, prevYVisible_);
    }

    prevXVisible_ = hasX;
    prevYVisible_ = hasY;
}

void NextAttackHintUI::ApplySprite(KetaEngine::Sprite* sprite, const Vector2& pos, bool& prevVisible) {
    sprite->transform_.pos   = pos;
    sprite->transform_.scale = spriteScale_;
    sprite->SetAlpha(alpha_);
    sprite->SetIsDraw(true);

    if (!prevVisible) {
        sprite->PlaySpriteEaseAnimation(spawnAnimName_, spawnAnimCategory_);
    }
}

void NextAttackHintUI::RegisterParams() {
    globalParameter_->Regist(groupName_, "worldOffset",  &worldOffset_);
    globalParameter_->Regist(groupName_, "spriteScale",  &spriteScale_);
    globalParameter_->Regist(groupName_, "xLocalOffset", &xLocalOffset_);
    globalParameter_->Regist(groupName_, "yLocalOffset", &yLocalOffset_);
    globalParameter_->Regist(groupName_, "alpha",        &alpha_);
}

void NextAttackHintUI::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat3("ワールドオフセット",        &worldOffset_.x,  0.05f);
        ImGui::DragFloat2("スプライトスケール",           &spriteScale_.x,  0.01f);
        ImGui::DragFloat2("X ローカルオフセット",       &xLocalOffset_.x, 1.0f);
        ImGui::DragFloat2("Y ローカルオフセット",       &yLocalOffset_.x, 1.0f);
        ImGui::SliderFloat("不透明度",                 &alpha_,          0.0f, 1.0f);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
