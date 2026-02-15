#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

/// <summary>
/// コンボアシストで使用するUIクラス
/// </summary>
class ComboAsistUI {
public:

    ComboAsistUI()  = default;
    ~ComboAsistUI() = default;

    enum class AsistState {
        NONE,
        PLAYING,
        NEXTATTACK,
    };

public:
    // 初期化、更新
    void Init();
    void Update();

private:
    // UI
    std::unique_ptr<KetaEngine::Sprite> bottomSprite_;
    std::unique_ptr<KetaEngine::Sprite> ArrowSprite_;

    // 発動中、次の攻撃用のアウトライン
    std::unique_ptr<KetaEngine::Sprite> outLineUI_;

    // アシストのステート
    AsistState state_ = AsistState::NONE;

public:
};
