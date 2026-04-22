#include "NextAttackIndicatorUI.h"

#include "2d/Sprite.h"
#include "3d/ViewProjection.h"
#include "Matrix4x4.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackAction.h"
#include "Player/Player.h"
#include "input/InputData.h"

#include <imgui.h>

// ボタンインデックス (GamepadButton enum の値と対応)
static constexpr int32_t kButtonA = 0;
static constexpr int32_t kButtonB = 1;
static constexpr int32_t kButtonX = 2;
static constexpr int32_t kButtonY = 3;

void NextAttackIndicatorUI::Init(Player* player, const KetaEngine::ViewProjection* viewProjection) {
    pPlayer_         = player;
    pViewProjection_ = viewProjection;

    // デフォルトテクスチャ設定（A/B/X/Y は既存テクスチャを使用。それ以外は空）
    static const char* kDefaultTextures[kButtonCount] = {
        "OperateA.dds", // A
        "OperateB.dds", // B
        "OperateX.dds", // X
        "OperateY.dds", // Y
        "",             // LB
        "",             // RB
        "",             // Back
        "",             // Start
        "",             // LS
        "",             // RS
        "",             // DPadUp
        "",             // DPadDown
        "",             // DPadLeft
        "",             // DPadRight
    };

    // デフォルトオフセット（ボタンを左右や上に少しずつずらして重ならないようにする）
    static const Vector3 kDefaultOffsets[4] = {
        {-1.0f, 2.5f, 0.0f}, // A
        { 1.0f, 2.5f, 0.0f}, // B
        {-1.0f, 3.5f, 0.0f}, // X
        { 1.0f, 3.5f, 0.0f}, // Y
    };

    for (int32_t i = 0; i < kButtonCount; ++i) {
        auto& entry           = entries_[i];
        entry.config.textureName = kDefaultTextures[i];

        // 顔ボタン(A/B/X/Y)はデフォルトオフセットを設定
        if (i < 4) {
            entry.config.worldOffset = kDefaultOffsets[i];
        }

        // テクスチャが設定されているボタンのみスプライトを生成
        if (!entry.config.textureName.empty()) {
            entry.sprite.reset(KetaEngine::Sprite::Create(entry.config.textureName, false));
            if (entry.sprite) {
                entry.sprite->SetIsDraw(false);
                entry.sprite->SetAnchorPoint({0.5f, 0.5f});
            }
        }
    }
}

void NextAttackIndicatorUI::Update() {
    // 全エントリを非表示にリセット
    for (auto& entry : entries_) {
        if (entry.sprite) {
            entry.sprite->SetIsDraw(false);
        }
    }

    if (!pPlayer_ || !pViewProjection_) {
        return;
    }

    // 現在のコンボアクションを取得
    auto* behavior = pPlayer_->GetComboBehavior();
    auto* action   = dynamic_cast<ComboAttackAction*>(behavior);
    if (!action) {
        return;
    }

    const auto& candidates = action->GetNextAttackCandidates();
    if (candidates.empty()) {
        return;
    }

    Vector3 playerPos = pPlayer_->GetWorldPosition();

    for (const auto& candidate : candidates) {
        if (!candidate.branch) {
            continue;
        }

        int32_t buttonIdx = candidate.branch->GetGamepadButton();
        if (buttonIdx < 0 || buttonIdx >= kButtonCount) {
            continue;
        }

        auto& entry = entries_[buttonIdx];
        if (!entry.sprite) {
            continue;
        }

        // ワールド座標 → スクリーン座標に変換
        Vector3 worldPos  = playerPos + entry.config.worldOffset;
        Vector2 screenPos = WorldToScreen(worldPos);

        entry.sprite->transform_.pos   = screenPos;
        entry.sprite->transform_.scale = entry.config.spriteScale;
        entry.sprite->SetAlpha(entry.config.alpha);
        entry.sprite->SetIsDraw(true);
    }
}

void NextAttackIndicatorUI::Draw() {
    for (auto& entry : entries_) {
        if (entry.sprite) {
            entry.sprite->Draw();
        }
    }
}

void NextAttackIndicatorUI::DrawEditorUI() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (!ImGui::CollapsingHeader("次の攻撃インジケーター")) {
        return;
    }

    ImGui::PushID("NextAttackIndicator");

    for (int32_t i = 0; i < kButtonCount; ++i) {
        auto& entry = entries_[i];

        // テクスチャが未設定のボタンはスキップ
        if (entry.config.textureName.empty()) {
            continue;
        }

        const char* buttonName = GetGamepadButtonName(i);
        if (ImGui::TreeNode(buttonName)) {
            ImGui::DragFloat3("ワールドオフセット", &entry.config.worldOffset.x, 0.1f);
            ImGui::DragFloat2("スプライトサイズ(px)", &entry.config.spriteScale.x, 1.0f, 1.0f, 512.0f);
            ImGui::SliderFloat("不透明度", &entry.config.alpha, 0.0f, 1.0f);
            ImGui::TreePop();
        }
    }

    ImGui::PopID();
#endif
}

Vector2 NextAttackIndicatorUI::WorldToScreen(const Vector3& worldPos) const {
    // ScreenTransform は Matrix4x4.h で定義済み (1280x720 スクリーン座標を返す)
    return ScreenTransform(worldPos, *pViewProjection_);
}
