#pragma once
#include "Scene/Factory/AbstractSceneFactory.h"
#include "Scene/Manager/SceneManager.h"
// utility
#include "Collider/CollisionManager.h"
#include "EngineCore.h"
/// std
#include <chrono>
#include <memory>

// ゲーム全体
namespace KetaEngine {

class KTFramework {
public:
    virtual ~KTFramework() = default;

    ///=======================================================
    /// public method
    ///=======================================================
    virtual void Init(); //< 初期化
    virtual void Update(); //< 更新
    virtual void Draw()           = 0; //< 描画
    virtual void DrawPostEffect() = 0; //< ポストエフェクト描画
    virtual void DrawShadow()     = 0; //< シャドウ描画
    virtual void Finalize(); //< 終了処理

    void Debug(); //< デバッグ
    void DisplayFPS(); //< FPS表示
    void DisplayGameView(); //< ゲームビュー表示
    void Run(); //< 実行

protected:
    /// other class
    SceneManager* pSceneManager_ = nullptr;

    ///=======================================================
    /// private variants
    ///=======================================================
    std::unique_ptr<AbstractSceneFactory> sceneFactory_;
    std::unique_ptr<EngineCore> engineCore_;

    // フレーム各フェーズの計測結果（ms）
    struct FrameTimings {
        float frameMs    = 0.0f; // フレーム全体
        float fixFpsMs   = 0.0f; // FixFPS (60fps캡)
        float beginMs    = 0.0f; // BeginFrame (WaitForNextFrame + ImGui + Input)
        float updateMs   = 0.0f; // Update (ゲームロジック + アニメーション)
        float drawMs     = 0.0f; // Draw (GPU コマンド積み)
        float endFrameMs = 0.0f; // EndFrame (ExecuteCommand + Present(1) + WaitForGPU)
        float workMs     = 0.0f; // FixFPS除いた実作業時間
    };
    FrameTimings frameTimings_;

private:
    void RunGpuWarmup(int numFrames); //< シーン遷移後 GPU P-state をP0に引き上げる
};

}; // KetaEngine