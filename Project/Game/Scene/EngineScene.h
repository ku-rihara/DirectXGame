#pragma once

/// base
#include "BaseScene.h"
/// 3D
#include "3D/Ground/Ground.h"
#include "3D/SkyBox/SkyBox.h"
/// std
#include <memory>

/// <summary>
/// エンジン機能確認用のシーン
/// SkyBoxと原点の地面だけを持つ空のシーン
/// </summary>
class EngineScene : public BaseScene {
public:
    EngineScene()           = default;
    ~EngineScene() override = default;

    ///========================================================
    /// public method
    ///========================================================

    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;
    void Debug() override;
    void ViewProcess() override;

private:
    ///========================================================
    /// private variant
    ///========================================================

    std::unique_ptr<KetaEngine::SkyBox> skyBox_ = nullptr;
    std::unique_ptr<KetaEngine::Ground> ground_ = nullptr;
};
