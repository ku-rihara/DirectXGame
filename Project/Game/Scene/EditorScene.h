#pragma once

/// base
#include "BaseScene.h"
// Audience
#include "Audience/AudienceController.h"
// Combo
#include "Combo/Combo.h"
// DeathTimer
#include "DeathTimer/DeathTimer.h"
// Enemy
#include "Enemy/EnemyManager.h"
#include "Enemy/Spawner/EnemySpawner.h"
// Field
#include "Field/Field.h"
#include "Field/SideRope/SideRopeController.h"
// Camera
#include "GameCamera/GameCamera.h"
// LockOn
#include "LockOn/LockOnController.h"
// Player
#include "Player/Effect/AttackEffect/AttackEffect.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
// SkyBox
#include "SkyBox/SkyBox.h"
// utility
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
    std::unique_ptr<SkyBox> skyBox_                                           = nullptr;
    std::unique_ptr<Combo> combo_                                             = nullptr;
    std::unique_ptr<EnemyManager> enemyManager_                               = nullptr;
    std::unique_ptr<EnemySpawner> enemySpawner_                               = nullptr;
    std::unique_ptr<AttackEffect> attackEffect_                               = nullptr;
    std::unique_ptr<PlayerComboAttackController> playerComboAttackController_ = nullptr;
    std::unique_ptr<SideRopeController> sideRopeController_                   = nullptr;
    std::unique_ptr<AudienceController> audienceController_                   = nullptr;
    std::unique_ptr<DeathTimer> deathTimer_                                   = nullptr;

    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender_ = nullptr;

public:
};