#pragma once

#include "Core/KTFramework.h"

namespace KetaEngine {

class KTGame : public KTFramework {
public:
    KTGame()  = default;
    ~KTGame() = default;

    ///======================================
    /// public method
    ///======================================
    void Init() override; //< 初期化
    void Update() override; //< 更新
    void Draw() override; //< 描画
    void DrawPostEffect() override; //< ポストエフェクト描画
    void DrawShadow() override; //< シャドウ描画
    void Finalize() override; //< 終了処理
};

}; // KetaEngine
