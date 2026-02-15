#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// 目的カットイン表示演出クラス
/// </summary>
class IntroPurposeCutIn : public BaseGameIntro {
public:
    // フェーズ
    enum class Phase {
        Appear, // 出現アニメーション中
        Close,  // 閉じアニメーション中
        Finish, // 完了
    };

    enum SpritePos {
        LEFT,
        RIGHT,
        CENTER,
        COUNT,
    };

public:
    IntroPurposeCutIn()           = default;
    ~IntroPurposeCutIn() override = default;

    void Init(const std::string& name) override;
    void Update(float playSpeed) override;

    // Param Edit
    void AdjustParam() override;
    void RegisterParams() override;
    void AdjustUniqueParam() override;

private:
    /// <summary>
    /// 全出現アニメーションが終了したかチェック
    /// </summary>
    bool IsAllAppearFinished() const;

    /// <summary>
    /// 閉じアニメーションを開始
    /// </summary>
    void StartCloseAnimation();

    /// <summary>
    /// 閉じアニメーションが終了したかチェック
    /// </summary>
    bool IsCloseFinished() const;

private:
    std::unique_ptr<KetaEngine::Sprite> backLineSprite_;
    std::array<std::unique_ptr<KetaEngine::Sprite>, static_cast<size_t>(SpritePos::COUNT)> sprites_;
    Phase phase_ = Phase::Appear;
};
