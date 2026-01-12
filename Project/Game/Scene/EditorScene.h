#pragma once

/// base
#include "BaseScene.h"

// object
#include "AttackEffect/AttackEffect.h"
#include "Audience/AudienceController.h"
#include "Combo/Combo.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/Spawner/EnemySpawner.h"
#include "Field/Field.h"
#include "Field/SideRope/SideRopeController.h"
#include "GameCamera/GameCamera.h"
#include "LockOn/LockOnController.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include "SkyBox/SkyBox.h"
#include "SkyDome/SkyDome.h"

#include "utility/ObjectFromBlender/ObjectFromBlender.h"

/// <summary>
/// ゲームシーン
/// </summary>
class EditorScene : public BaseScene {
public:
    EditorScene()           = default;
    ~EditorScene() override = default;

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

private:
    ///========================================================
    /// private method
    ///========================================================

    void ObjectInit();
    void SetClassPointer();

private:
    //*-------------------------------- Private variants--------------------------------*//

    std::unique_ptr<GameCamera> gameCamera_                                   = nullptr;
    std::unique_ptr<Field> field_                                             = nullptr;
    std::unique_ptr<LockOnController> lockOnController_                       = nullptr;
    std::unique_ptr<Player> player_                                           = nullptr;
    std::unique_ptr<SkyDome> skyDome_                                         = nullptr;
    std::unique_ptr<SkyBox> skyBox_                                           = nullptr;
    std::unique_ptr<Combo> combo_                                             = nullptr;
    std::unique_ptr<EnemyManager> enemyManager_                               = nullptr;
    std::unique_ptr<EnemySpawner> enemySpawner_                               = nullptr;
    std::unique_ptr<AttackEffect> attackEffect_                               = nullptr;
    std::unique_ptr<PlayerComboAttackController> playerComboAttackController_ = nullptr;
    std::unique_ptr<SideRopeController> sideRopeController_                   = nullptr;
    std::unique_ptr<AudienceController> audienceController_                   = nullptr;

    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender_ = nullptr;

public:
};