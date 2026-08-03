#pragma once
#include "Scene/Factory/AbstractSceneFactory.h"
#include "Scene/Manager/SceneManager.h"
// utility
#include "EngineCore.h"
/// std
#include <memory>

// ゲーム全体
namespace KetaEngine {

/// <summary>
/// アプリケーション全体のInit/Update/Drawライフサイクルを定義する基底フレームワーククラス
/// </summary>
class KTFramework {
public:
    virtual ~KTFramework() = default;

    ///=======================================================
    /// public method
    ///=======================================================

    // 初期化、更新、描画
    virtual void Init();
    virtual void Update();
    virtual void Draw() = 0;

    // ポストエフェクト、シャドウ描画
    virtual void DrawPostEffect() = 0;
    virtual void DrawShadow()     = 0;

    // 終了処理
    virtual void Finalize();

    // デバッグ表示
    void Debug();
    void DisplayFPS();

    // ゲームビュー表示、実行
    void DisplayGameView();
    void Run();

protected:
    /// other class
    SceneManager* pSceneManager_ = nullptr;

    ///=======================================================
    /// private variants
    ///=======================================================
    std::unique_ptr<AbstractSceneFactory> sceneFactory_;
    std::unique_ptr<EngineCore> engineCore_;

    /// <summary>
    /// フレーム各フェーズの計測結果
    /// </summary>
    struct FrameTimings {
        float frameMs    = 0.0f; 
        float fixFpsMs   = 0.0f; 
        float beginMs    = 0.0f; 
        float updateMs   = 0.0f; 
        float drawMs     = 0.0f; 
        float endFrameMs = 0.0f; 
        float workMs     = 0.0f; 
    };
    FrameTimings frameTimings_;

private:
    //< シーン遷移後 GPU P-state をP0に引き上げる
    void RunGpuWarmup(int numFrames);
};

}; // KetaEngine