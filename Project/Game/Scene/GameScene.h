#pragma once

/// base
#include "BaseScene.h"

// Obj
#include "SkyBox/SkyBox.h"
// emitter

#include "Editor/CameraEditor/CameraAnimation.h"
#include "utility/ObjectFromBlender/ObjectFromBlender.h"
#include "utility/TimeLine/TimelineDrawer.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
private:
    ///========================================================
    /// Private variants
    ///========================================================

    std::unique_ptr<SkyBox> skuBox_                                  = nullptr;
    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender = nullptr;
    std::unique_ptr<KetaEngine::CameraAnimation> cameraPlayer_       = nullptr;
    std::unique_ptr<KetaEngine::TimelineDrawer> timeLine_            = nullptr;

    public : GameScene();
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