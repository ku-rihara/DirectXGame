#pragma once

/// baseScene
#include "BaseScene.h"

#include "Easing/Easing.h"
#include "EasingTestObj/EasingTestObj.h"
#include "Editor/EasingCreator/Easingeditor.h"
#include "GPUParticle/GPUParticleEmitter.h"
#include <memory>

/// <summary>
/// エディターシーン
/// </summary>
class EditorScene : public BaseScene {
private:
    std::unique_ptr<EasingTestObj> easingTestObject_;
    EasingEditor easingEditor_;

    // GPUパーティクルエミッター
    std::unique_ptr<GPUParticleEmitter> testGPUParticle_;
    std::unique_ptr<GPUParticleEmitter> fireEffect_;
    std::unique_ptr<GPUParticleEmitter> explosionEffect_;

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