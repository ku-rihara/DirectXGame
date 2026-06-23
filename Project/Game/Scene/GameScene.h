#pragma once

/// base
#include "BaseScene.h"
#include "GameSceneState/BaseGameSceneState.h"
#include "GameObj.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
public:
    GameScene()           = default;
    ~GameScene() override = default;

    ///========================================================
    /// public method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;

    void Debug() override;
    void ViewProjectionUpdate() override;
    void ViewProcess() override;

    void ChangeState(std::unique_ptr<BaseGameSceneState> state);

private:
    ///========================================================
    /// private method
    ///========================================================

    void ObjectInit();
    void SetClassPointer();

private:
    //*-------------------------------- Private variants--------------------------------*//

    std::unique_ptr<BaseGameSceneState> state_ = nullptr;
    GameObj gameObj_;


public:
    GameObj& GetGameObj() { return gameObj_; }
    KetaEngine::DitherOcclusion* GetDitherOcclusion() { return gameObj_.backGroundObjectManager_->GetDitherOcclusion(); }
};