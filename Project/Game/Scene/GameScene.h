#pragma once

/// base
#include "BaseScene.h"

// object
#include "AttackEffect/AttackEffect.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "Combo/Combo.h"
#include "ComboCreate/ComboCreateEditor.h"
#include "ComboScene/ComboScene.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/Spawner/EnemySpawner.h"
#include "Field/Field.h"
#include "FireInjectors/FireInjectors.h"
#include "GameCamera/GameCamera.h"
#include "LockOn/LockOn.h"
#include "Player/Player.h"
#include "SkyBox/SkyBox.h"
#include "SkyDome/SkyDome.h"
#include "UI/HowToOperate.h"
#include "utility/CameraEditor/CameraEditor.h"
#include "utility/ShakeEditor/ShakeEditor.h"
#include "GameIntro/GameIntro.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
public:
    GameScene()           = default;
    ~GameScene() override = default;

    ///========================================================
    /// private method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void ModelDraw() override;
    void SpriteDraw() override;
    void SkyBoxDraw() override;
    void DrawShadow() override;

    void Debug() override;
    void ViewProjectionUpdate() override;
    void ViewProssess() override;

    void ChangeForJoyState();

private:
    ///========================================================
    /// Private variants
    ///========================================================

    ///* objects
    std::unique_ptr<GameCamera> gameCamera_                     = nullptr;
    std::unique_ptr<Field> field_                               = nullptr;
    std::unique_ptr<LockOn> lockOn_                             = nullptr;
    std::unique_ptr<Player> player_                             = nullptr;
    std::unique_ptr<Skydome> skyDome_                           = nullptr;
    std::unique_ptr<SkyBox> skyBox_                             = nullptr;
    std::unique_ptr<Combo> combo_                               = nullptr;
    std::unique_ptr<EnemyManager> enemyManager_                 = nullptr;
    std::unique_ptr<EnemySpawner> enemySpawner_                 = nullptr;
    std::unique_ptr<HowToOperate> howToOperate_                 = nullptr;
    std::unique_ptr<ComboCreateEditor> comboCreate_             = nullptr;
    std::unique_ptr<FireInjectors> fireInjectors_               = nullptr;
    std::unique_ptr<GameBackGroundObject> gameBackGroundObject_ = nullptr;
    std::unique_ptr<ComboScene> comboScene_                     = nullptr;
    std::unique_ptr<CameraEditor> cameraEditor_                 = nullptr;
    std::unique_ptr<ShakeEditor> shakeEditor_                   = nullptr;
    std::unique_ptr<AttackEffect> attackEffect_                 = nullptr;

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