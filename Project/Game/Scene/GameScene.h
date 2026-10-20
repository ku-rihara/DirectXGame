#pragma once

/// base
#include "BaseScene.h"
/// 3D
#include "3D/Ground/Ground.h"
#include "3D/SkyBox/SkyBox.h"
/// editor
#include "Editor/AudioDSPEditor/AudioDSPEditor.h"
/// std
#include <memory>

/// <summary>
/// ゲームシーン
/// サウンド編集(音声合成・フィルター)エディターを開いて試せる
/// </summary>
class GameScene : public BaseScene {
public:
    GameScene()           = default;
    ~GameScene() override = default;

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

    // 波形加工(音声合成・フィルター)エディター
    std::unique_ptr<KetaEngine::AudioDSPEditor> audioDSPEditor_ = nullptr;
};
