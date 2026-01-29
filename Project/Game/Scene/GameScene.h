#pragma once

/// base
#include "BaseScene.h"
#include "GameSceneState/BaseGameSceneState.h"

// Audience
#include "Audience/AudienceController.h"
// BackGroundObj
#include "BackGroundObject/ComboLevel/ComboLevelObjHolder.h"
#include "BackGroundObject/GameBackGroundObject.h"
// Combo
#include "Combo/Combo.h"
#include "ComboScene/ComboScene.h"
// DeathTimer
#include "DeathTimer/DeathTimer.h"
// Enemy
#include "Enemy/EnemyManager.h"
#include "Enemy/Spawner/ContinuousEnemySpawner.h"
#include "Enemy/Spawner/EnemySpawner.h"
// Field
#include "Field/Field.h"
#include "Field/SideRope/SideRopeController.h"
#include "FireInjectors/FireInjectors.h"
// Camera
#include "GameCamera/GameCamera.h"
// Intro
#include "GameIntro/GameIntroManager.h"
// LockOn
#include "LockOn/LockOnController.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Effect/AttackEffect/AttackEffect.h"
#include "Player/Player.h"
// SkyBox
#include "SkyBox/SkyBox.h"
// UI
#include "UI/HowToOperate.h"
// Sprite
#include "2D/Sprite.h"

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

    struct GameObj {
        std::unique_ptr<GameCamera> gameCamera_                                   = nullptr;
        std::unique_ptr<Field> field_                                             = nullptr;
        std::unique_ptr<LockOnController> lockOnController_                       = nullptr;
        std::unique_ptr<Player> player_                                           = nullptr;
        std::unique_ptr<SkyBox> skyBox_                                           = nullptr;
        std::unique_ptr<Combo> combo_                                             = nullptr;
        std::unique_ptr<EnemyManager> enemyManager_                               = nullptr;
        std::unique_ptr<EnemySpawner> enemySpawner_                               = nullptr;
        std::unique_ptr<HowToOperate> howToOperate_                               = nullptr;
        std::unique_ptr<FireInjectors> fireInjectors_                             = nullptr;
        std::unique_ptr<GameBackGroundObject> gameBackGroundObject_               = nullptr;
        std::unique_ptr<ComboScene> comboScene_                                   = nullptr;
        std::unique_ptr<AttackEffect> attackEffect_                               = nullptr;
        std::unique_ptr<GameIntroManager> gameIntroManager_                       = nullptr;
        std::unique_ptr<ComboLevelObjHolder> comboLevelObjHolder_                 = nullptr;
        std::unique_ptr<ContinuousEnemySpawner> continuousEnemySpawner_           = nullptr;
        std::unique_ptr<PlayerComboAttackController> playerComboAttackController_ = nullptr;
        std::unique_ptr<SideRopeController> sideRopeController_                   = nullptr;
        std::unique_ptr<AudienceController> audienceController_                   = nullptr;
        std::unique_ptr<DeathTimer> deathTimer_                                   = nullptr;

        std::unique_ptr<KetaEngine::Sprite> screenSprite_;
    };

    std::unique_ptr<BaseGameSceneState> state_ = nullptr;
    GameObj gameObj_;

public:
    GameObj& GetGameObj() { return gameObj_; }
};