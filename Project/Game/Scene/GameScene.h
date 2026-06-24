#pragma once

/// base
#include "BaseScene.h"
#include "GameSceneState/BaseGameSceneState.h"
#include "GameObj.h"

/// <summary>
/// ゲームシーン
/// ステートパターンで Intro / Playing / Pause / GameOver を切り替える
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
    void ViewProjectionUpdate() override;
    void ViewProcess() override;

    /// 現在のステートを切り替える
    void ChangeState(std::unique_ptr<BaseGameSceneState> state);

private:
    ///========================================================
    /// private method
    ///========================================================

    void ObjectInit();    ///< 各オブジェクトの生成・初期化
    void SetClassPointer(); ///< オブジェクト間のポインタ接続

private:
    std::unique_ptr<BaseGameSceneState> state_ = nullptr; ///< 現在のシーンステート
    GameObj gameObj_;                                      ///< ゲーム内全オブジェクト

public:
    GameObj& GetGameObj() { return gameObj_; }
    KetaEngine::DitherOcclusion* GetDitherOcclusion() { return gameObj_.backGroundObjectManager_->GetDitherOcclusion(); }
};
