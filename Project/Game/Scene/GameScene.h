#pragma once

/// base
#include "BaseScene.h"

// object
#include "AttackEffect/AttackEffect.h"
#include "Audience/AudienceController.h"
#include "BackGroundObject/ComboLeve/ComboLevelObjHolder.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "Combo/Combo.h"
#include "ComboScene/ComboScene.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/Spawner/ContinuousEnemySpawner.h"
#include "Enemy/Spawner/EnemySpawner.h"
#include "Field/Field.h"
#include "Field/SideRope/SideRopeController.h"
#include "FireInjectors/FireInjectors.h"
#include "GameCamera/GameCamera.h"
#include "GameIntro/GameIntroManager.h"
#include "LockOn/LockOnController.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include "SkyBox/SkyBox.h"
#include "SkyDome/SkyDome.h"
#include "UI/HowToOperate.h"
#include "DeathTimer/DeathTimer.h"

#include "GameSceneState/BaseGameSceneState.h"

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
        std::unique_ptr<SkyDome> skyDome_                                         = nullptr;
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
    };

    std::unique_ptr<BaseGameSceneState> state_ = nullptr;
    GameObj gameObj_;

public:
    GameObj& GetGameObj() { return gameObj_; }
   /* const ViewProjection& GetViewProjection() const { return viewProjection_; }*/
};