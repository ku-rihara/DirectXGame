#pragma once

/// baseScene
#include "BaseScene.h"

#include "Easing/Easing.h"
#include "EasingTestObj/EasingTestObj.h"
#include "utility/EasingCreator/Easingeditor.h"
#include <memory>
/// <summary>
/// EditorScene
/// </summary>
class EditorScene : public BaseScene {
private:
    std::unique_ptr<EasingTestObj> easingTestObject_;
    EasingEditor easingEditor_;

public:
    ///========================================================
    /// Constralt destract
    ///========================================================
    EditorScene();
    ~EditorScene() override;

    ///========================================================
    /// private method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;

    void Debug() override; /// debug
    void ViewProjectionUpdate() override;
    void ViewProcess() override;
};