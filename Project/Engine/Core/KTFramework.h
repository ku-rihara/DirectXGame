#pragma once
#include "Scene/Factory/AbstractSceneFactory.h"
#include "Scene/Manager/SceneManager.h"
// utility
#include "Collider/CollisionManager.h"
#include "EngineCore.h"
/// std
#include <memory>

// ゲーム全体
namespace KetaEngine {

class KTFramework {
public:
    virtual ~KTFramework() = default;

    ///=======================================================
    /// public method
    ///=======================================================
    virtual void Init();               //< 初期化
    virtual void Update();             //< 更新
    virtual void Draw()           = 0; //< 描画
    virtual void DrawPostEffect() = 0; //< ポストエフェクト描画
    virtual void DrawShadow()     = 0; //< シャドウ描画
    virtual void Finalize();           //< 終了処理

    void Debug();      //< デバッグ
    void DisplayFPS(); //< FPS表示
    void Run();        //< 実行

protected:
    /// other class
    SceneManager* pSceneManager_ = nullptr;

    ///=======================================================
    /// private variants
    ///=======================================================
    std::unique_ptr<AbstractSceneFactory> sceneFactory_;
    std::unique_ptr<CollisionManager> collisionManager_;
    std::unique_ptr<EngineCore> engineCore_;
};

}; // KetaEngine
