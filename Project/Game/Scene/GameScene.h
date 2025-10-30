#pragma once

/// base
#include "BaseScene.h"

// object
#include "AttackEffect/AttackEffect.h"
#include "BackGroundObject/ComboLeve/ComboLevelObjHolder.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "Combo/Combo.h"
#include "ComboScene/ComboScene.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/Spawner/ContinuousEnemySpawner.h"
#include "Enemy/Spawner/EnemySpawner.h"
#include "Field/Field.h"
#include "FireInjectors/FireInjectors.h"
#include "GameCamera/GameCamera.h"
#include "GameIntro/GameIntroManager.h"
#include "LockOn/LockOnController.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include "SkyBox/SkyBox.h"
#include "SkyDome/SkyDome.h"
#include "UI/HowToOperate.h"
#include "utility/CameraEditor/CameraEditor.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
public:
    enum class GameState {
        INTRO,
        PLAY,
        FINISH,
    };

public:
    GameScene()           = default;
    ~GameScene() override = default;

    ///========================================================
    /// private method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;

    void ObjectInit();
    void SetClassPointer();

    // state Update
    void IntroUpdate();
    void PlayUpdate();
    void FinishUpdate();

    void Debug() override;
    void ViewProjectionUpdate() override;
    void ViewProcess() override;

    void ChangeForJoyState();

private:
    ///========================================================
    /// Private variants
    ///========================================================

    // objects
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

    GameState gameState_ = GameState::INTRO;

    bool isDraw = true;

    Vector3 lightPos_;

    uint32_t defaultHandle_;
    uint32_t circleHandle_;
    uint32_t uv_;

    bool isfirstChange_;
    float alpha_;
    int shandle_;
    std::unique_ptr<Sprite> screenSprite_;

    int chandle_;
    Easing<Vector2> finishSpriteEase_;
    Vector2 tempSpritePos_;
    std::unique_ptr<Sprite> cSprite_;
    bool isend_;
};