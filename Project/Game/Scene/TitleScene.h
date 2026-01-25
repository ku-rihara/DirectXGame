#pragma once

/// baseScene
#include "BaseScene.h"

// objects
#include "Field/Field.h"
#include "Player/Player.h"
#include "SkyBox/SkyBox.h"
#include "SkyDome/SkyDome.h"

// utility
#include "utility/ObjectFromBlender/ObjectFromBlender.h"

// TitleSceneController
#include "TitleSceneController/TitleSceneController.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public BaseScene {
private:
  
    std::unique_ptr<Player> player_                                   = nullptr;
    std::unique_ptr<Field> field_                                     = nullptr;
    std::unique_ptr<SkyBox> skyBox_                                   = nullptr;
    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender_ = nullptr;

    // 演出・遷移の統括管理
    std::unique_ptr<TitleSceneController> sceneController_ = nullptr;

public:
    TitleScene();
    ~TitleScene() override;

    ///========================================================
    /// public method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;

    void Debug() override; /// debug
    void ViewProjectionUpdate() override;
    void ViewProcess() override;
};