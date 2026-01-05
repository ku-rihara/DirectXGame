#pragma once

/// base
#include "BaseScene.h"

// Obj
#include "Editor/TimeScaleEditor/TimeScaleController.h"
#include "SkyBox/SkyBox.h"
// emitter

#include "utility/ObjectFromBlender/ObjectFromBlender.h"

#include "Editor/CameraEditor/CameraAnimation.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
private:
    ///========================================================
    /// Private variants
    ///========================================================

    std::unique_ptr<SkyBox> skuBox_                                       = nullptr;
    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender        = nullptr;
    std::unique_ptr<KetaEngine::TimeScaleController> timeScaleController_ = nullptr;
    std::unique_ptr<KetaEngine::CameraAnimation> cameraPlayer_            = nullptr;

public:
    GameScene();
    ~GameScene() override;

    ///========================================================
    /// private method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;

    void Debug() override;
    void ViewProjectionUpdate() override;
    void ViewProcess() override;
};