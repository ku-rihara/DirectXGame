#pragma once

/// baseScene
#include "BaseScene.h"

#include "Easing/Easing.h"
#include "EasingTestObj/EasingTestObj.h"
#include "Editor/EasingCreator/Easingeditor.h"
#include <memory>

/// <summary>
/// エディターシーン
/// </summary>
class EditorScene : public BaseScene {
private:
    std::unique_ptr<EasingTestObj> easingTestObject_;
    KetaEngine::EasingEditor easingEditor_;

public:
    EditorScene();
    ~EditorScene() override;

    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;

    void Debug() override;
    void ViewProjectionUpdate() override;
    void ViewProcess() override;
};