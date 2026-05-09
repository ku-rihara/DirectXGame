#pragma once
#include "Vector3.h"
#include <array>
#include <string>

namespace KetaEngine {
class GlobalParameter;
}

/// <summary>
/// 敵HPバーの段階別色設定
/// </summary>
class EnemyHPBarColorConfig {
public:
    struct Stage {
        float threshold = 0.0f; ///< この割合以上なら本ステージの色を使用
        Vector3 color   = {1.0f, 1.0f, 1.0f};
    };

    void Init();
    void AdjustParam();
    void RegisterParams();

    /// <summary>
    /// HP割合から対応する色を返す
    /// </summary>
    Vector3 GetColor(float ratio) const;

private:
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "EnemyHPBarColor";

    // [0]=高HP(緑), [1]=中HP(黄), [2]=低HP(赤) 
    std::array<Stage, 3> stages_ = {{
        {0.6f, {0.18f, 0.85f, 0.18f}},
        {0.3f, {0.95f, 0.85f, 0.10f}},
        {0.0f, {0.90f, 0.10f, 0.10f}},
    }};
};
