#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

/// <summary>
/// タイトルロゴ管理クラス
/// </summary>
class TitleLogo {
public:
    // 初期化、更新
    void Init();
    void Update();
private:
    enum class Step {
        PUNCH,
        RUSH,
        BOTTON,
        END,
    };

private:
    Step step_;
    // スプライト
    std::unique_ptr<KetaEngine::Sprite> punchSprite_;
    std::unique_ptr<KetaEngine::Sprite> rushSprite_;
    std::unique_ptr<KetaEngine::Sprite> bottomSprite_;
};
