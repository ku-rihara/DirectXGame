#pragma once

#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackAction.h"
#include "Vector2.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// 次の攻撃ボタンを画面上に表示するUIクラス
/// </summary>
class NextAttackUI {
public:
    static constexpr int32_t kMaxCandidates = 4;

public:
    NextAttackUI()  = default;
    ~NextAttackUI() = default;

    void Init();
    void Update(float deltaTime);

    /// <summary>
    /// 次の攻撃候補を渡してOpen（SpriteEasing "NextAttackOpen" 再生）
    /// </summary>
    void Open(const std::vector<ComboAttackAction::NextAttackCandidate>& candidates);

    /// <summary>
    /// Close（SpriteEasing "NextAttackClose" 再生→一定時間後に非表示）
    /// 攻撃がRootに戻ったタイミングで呼ぶ
    /// </summary>
    void Close();

    void AdjustParam();
    void RegisterParams();

private:
    static std::string GetButtonTexture(int32_t gamepadButton);

    void UpdateSlotTransforms();
    void HideAllSlots();

private:
    enum class State { Hidden, Showing, Closing };

    struct ButtonSlot {
        std::unique_ptr<KetaEngine::Sprite> sprite;
        bool isActive = false;
    };

    std::array<ButtonSlot, kMaxCandidates> slots_;

    State state_             = State::Hidden;
    float closeTimer_        = 0.0f;
    float closeAnimDuration_ = 0.3f;  // Close animationが終わるまでの待ち時間

    // レイアウト
    Vector2 basePos_;
    std::array<Vector2, kMaxCandidates> slotOffsets_;
    Vector2 spriteScale_ = {64.0f, 64.0f};

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_  = "NextAttackUI";
    const std::string folderPath_ = "NextAttackUI";
};
