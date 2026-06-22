#pragma once

/// baseScene
#include "BaseScene.h"

// objects
#include "Field/Field.h"
#include "Player/TitlePlayer/TitlePlayer.h"
#include "SkyBox/SkyBox.h"
#include "SkyDome/SkyDome.h"

// utility
#include "utility/ObjectFromBlender/ObjectFromBlender.h"

// TitleSceneController
#include "TitleSceneController/TitleSceneController.h"

// editor
#include "Editor/ParameterEditor/GlobalParameter.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public BaseScene {
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

    void ViewProjectionUpdate() override;
    void ViewProcess() override;

    // デバッグ
    void Debug() override;

private:
    // 編集するパラメータ登録
    void RegisterParams();

private:
    std::unique_ptr<TitlePlayer> player_                              = nullptr;
    std::unique_ptr<Field> field_                                     = nullptr;
    std::unique_ptr<SkyBox> skyBox_                                   = nullptr;
    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender_ = nullptr;

    // 演出・遷移の統括管理
    std::unique_ptr<TitleSceneController> sceneController_ = nullptr;

    // GlobalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "TitleScene";
};