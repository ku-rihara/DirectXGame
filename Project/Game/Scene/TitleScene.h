#pragma once

/// baseScene
#include "BaseScene.h"

#include "Particle/CPUParticle/ParticlePlayer.h"
#include "Particle/GPUParticle/GPUParticlePlayer.h"
#include <array>
#include <string>

/// <summary>
///  タイトルシーン
/// </summary>
class TitleScene : public BaseScene {
private:
    KetaEngine::GPUParticlePlayer test;
    KetaEngine::ParticlePlayer particleTestPlayer_;

public:
    TitleScene();
    ~TitleScene() override;

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