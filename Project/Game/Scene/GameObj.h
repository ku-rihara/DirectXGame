#pragma once
#include "Audience/AudienceController.h"
#include "BackGroundObject/BackGroundObjectManager.h"
#include "Combo/Combo.h"
#include "ComboScene/ComboDirector.h"
#include "Enemy/EnemyManager/EnemyManager.h"
#include "Enemy/EnemyManager/Spawner/ContinuousEnemySpawner.h"
#include "Field/Field.h"
#include "Field/SideRope/SideRopeController.h"
#include "FireInjectors/FireInjectors.h"
#include "GameCamera/GameCamera.h"
#include "GameIntro/GameIntroManager.h"
#include "KillCounter/KillCounter.h"
#include "LockOn/LockOnController.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include "SkyBox/SkyBox.h"
#include "StressGauge/StressGauge.h"
#include "UI/ComboAssistUI/ComboAssistController.h"
#include "UI/ComboAssistUI/SupportSprite/ComboSupportSpriteUi.h"
#include "UI/ComboAssistUI/UnlockNotifier/ComboUnlockNotifier.h"
#include "UI/NextAttackIndicator/NextAttackHintUI.h"
#include "UI/OperateUI.h"
#include "2D/Sprite.h"
#include <memory>

struct GameObj {
    std::unique_ptr<GameCamera>                   gameCamera_                   = nullptr;
    std::unique_ptr<Field>                        field_                        = nullptr;
    std::unique_ptr<LockOnController>             lockOnController_             = nullptr;
    std::unique_ptr<Player>                       player_                       = nullptr;
    std::unique_ptr<SkyBox>                       skyBox_                       = nullptr;
    std::unique_ptr<Combo>                        combo_                        = nullptr;
    std::unique_ptr<EnemyManager>                 enemyManager_                 = nullptr;
    std::unique_ptr<OperateUI>                    operateUI_                    = nullptr;
    std::unique_ptr<FireInjectors>                fireInjectors_                = nullptr;
    std::unique_ptr<BackGroundObjectManager>      backGroundObjectManager_      = nullptr;
    std::unique_ptr<ComboDirector>                comboDirector_                = nullptr;
    std::unique_ptr<GameIntroManager>             gameIntroManager_             = nullptr;
    std::unique_ptr<ContinuousEnemySpawner>       continuousEnemySpawner_       = nullptr;
    std::unique_ptr<PlayerComboAttackController>  playerComboAttackController_  = nullptr;
    std::unique_ptr<SideRopeController>           sideRopeController_           = nullptr;
    std::unique_ptr<AudienceController>           audienceController_           = nullptr;
    std::unique_ptr<StressGauge>                  StressGauge_                  = nullptr;
    std::unique_ptr<KillCounter>                  killCounter_                  = nullptr;
    std::unique_ptr<ComboAssistController>        comboAssistController_        = nullptr;
    std::unique_ptr<ComboUnlockNotifier>          unlockNotifier_               = nullptr;
    std::unique_ptr<ComboSupportSpriteUi>         comboSupportSpriteUi_         = nullptr;
    std::unique_ptr<NextAttackHintUI>             nextAttackHintUI_             = nullptr;
    std::unique_ptr<KetaEngine::Sprite>           screenSprite_;
};
