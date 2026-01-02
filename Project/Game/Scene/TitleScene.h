#pragma once

/// baseScene
#include "BaseScene.h"

#include "Editor/GPUParticleEditor/GPUParticlePlayer.h"
#include "Editor/ParticleEditor/ParticleEmitter.h"
#include "Editor/ParticleEditor/ParticlePlayer.h"
#include <array>
#include <string>

/// <summary>
///  タイトルシーン
/// </summary>
class TitleScene : public BaseScene {
private:
    std::array<std::unique_ptr<KetaEngine::ParticleEmitter>, 3> EnemydamageEffect_;
    std::array<std::unique_ptr<KetaEngine::ParticleEmitter>, 1> afterGlowEffect_;
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