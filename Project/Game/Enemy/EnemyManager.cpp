#include "EnemyManager.h"
// UIs
#include "UIs/GroupIcon/GroupIcon.h"
// Math
#include "Matrix4x4.h"
// Types
#include "Types/NormalEnemy.h"
#include "Types/StrongEnemy.h"
// Behavior
#include "Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
// Player
#include "Player/Player.h"
// DeathTimer
#include "DeathTimer/DeathTimer.h"
// Combo
#include "Combo/Combo.h"
// LockOn
#include "LockOn/LockOn.h"
// Spawner
#include "Spawner/EnemySpawner.h"
// imGui
#include <imgui.h>

///========================================================================================
///  初期化
///========================================================================================
void EnemyManager::Init() {
    selectedEnemyType_ = "NormalEnemy";
    spawnPosition_     = {};

    damageReactionController_ = std::make_unique<EnemyDamageReactionController>();
    damageReactionController_->Init();

    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    hpBarColorConfig_.Init();
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
    const Vector3& localOffset, const std::string& parentBossName) {

    std::unique_ptr<BaseEnemy> enemy;

    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) {
        enemy = std::make_unique<NormalEnemy>();
        enemy->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
    } else if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)] || enemyType == "BossEnemy") {
        auto strong = std::make_unique<StrongEnemy>();
        strong->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
        strong->SetStrongParameter(strongEnemyParam_);
        enemy = std::move(strong);
    }

    if (!enemy) {
        return;
    }

    enemy->SetPlayer(pPlayer_);
    enemy->SetGameCamera(pGameCamera_);
    enemy->SetManager(this);
    enemy->SetCombo(pCombo_);
    enemy->SetKillCounter(pKillCounter_);
    enemy->SetGroupId(groupID);
    enemy->Init(position);
    enemy->SetHPBarColorConfig(&hpBarColorConfig_);
    UpdateAvailableAnimationsForEditor(enemy.get());

    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetSpawnOffset(localOffset);
        ne->SetNormalParameter(normalEnemyParam_);
    }

    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetOnDamageTakenCallback([this, ne]() {
            if (pDeathTimer_ && ne->GetZakoState() == NormalEnemy::ZakoState::CrawlBackwards) {
                pDeathTimer_->OnNormalEnemyHit();
            }
        });
    }

    if (enemy->GetType() == BaseEnemy::Type::STRONG && !parentBossName.empty()) {
        bossByName_[parentBossName] = enemy.get();
    }

    NormalEnemy* newMinion = nullptr;
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        newMinion = static_cast<NormalEnemy*>(enemy.get());
    }

    enemies_.push_back(std::move(enemy));

    LinkBossAndMinions(groupID, newMinion, parentBossName);
}

///========================================================================================
///  ボス・取り巻きリンク
///========================================================================================
void EnemyManager::LinkBossAndMinions(int32_t groupID, NormalEnemy* newMinion, const std::string& parentBossName) {

    for (auto& e : enemies_) {
        if (e->GetGroupId() == groupID && e->GetType() == BaseEnemy::Type::STRONG) {
            if (minionsByBoss_.find(e.get()) == minionsByBoss_.end()) {
                minionsByBoss_[e.get()]  = {};
                bossGroupIndex_[e.get()] = nextBossGroupIndex_++;
            }
        }
    }

    if (!newMinion) {
        return;
    }
    if (newMinion->GetBoss() != nullptr) {
        return;
    }

    BaseEnemy* targetBoss = nullptr;

    if (!parentBossName.empty()) {
        auto it = bossByName_.find(parentBossName);
        if (it != bossByName_.end()) {
            targetBoss = it->second;
        }
    }

    if (!targetBoss) {
        for (auto& enemy : enemies_) {
            if (enemy->GetGroupId() == groupID && enemy->GetType() == BaseEnemy::Type::STRONG) {
                targetBoss = enemy.get();
                break;
            }
        }
    }

    if (!targetBoss) {
        return;
    }

    newMinion->SetBoss(targetBoss);

    auto idxIt = bossGroupIndex_.find(targetBoss);
    if (idxIt != bossGroupIndex_.end()) {
        int32_t iconIdx = static_cast<int32_t>(idxIt->second % GroupIcon::kGroupCount);
        newMinion->SetGroupIconIndex(iconIdx);
        targetBoss->SetGroupIconIndex(iconIdx);
    }

    const Vector3& knownOffset = newMinion->GetSpawnOffset();
    bool hasPresetOffset       = (knownOffset.x != 0.0f || knownOffset.z != 0.0f);
    if (!hasPresetOffset) {
        Vector3 offset = newMinion->GetWorldPosition() - targetBoss->GetWorldPosition();
        offset.y       = 0.0f;
        newMinion->SetSpawnOffset(offset);
    }

    minionsByBoss_[targetBoss].push_back(newMinion);
}

///========================================================================================
///  煽り状態更新（毎フレーム）
///========================================================================================
void EnemyManager::UpdateTauntState() {
    if (!pPlayer_ || !pDeathTimer_) {
        return;
    }

    Vector3 playerPos   = pPlayer_->GetBaseTransform().GetWorldPos();
    bool anyBossInRange = false;
    int32_t tauntCount  = 0;

    for (auto& [boss, minions] : minionsByBoss_) {
        if (!boss || boss->GetIsDeath()) {
            continue;
        }

        float dist   = (boss->GetWorldPosition() - playerPos).Length();
        bool inRange = (dist <= bossPlayerTriggerDistance_);

        // StrongEnemyの煽り状態も更新
        if (StrongEnemy* strong = dynamic_cast<StrongEnemy*>(boss)) {
            if (inRange) {
                strong->StartTaunt();
            } else {
                strong->StopTaunt();
            }
        }

        for (NormalEnemy* zako : minions) {
            if (!zako || zako->GetIsDeath()) {
                continue;
            }

            if (inRange) {
                zako->StartTaunt();
                if (zako->IsTaunting()) {
                    ++tauntCount;
                    anyBossInRange = true;
                }
            } else {
                zako->StopTaunt();
            }
        }
    }

    pDeathTimer_->SetTauntState(anyBossInRange, tauntCount);
}

///========================================================================================
///  ボス撃破処理
///========================================================================================
void EnemyManager::OnBossKilled(BaseEnemy* dyingBoss) {

    auto it = minionsByBoss_.find(dyingBoss);
    if (it != minionsByBoss_.end()) {
        for (NormalEnemy* zako : it->second) {
            if (zako && !zako->GetIsDeath()) {
                zako->StartFlee();
            }
        }
        minionsByBoss_.erase(it);
    }
    bossGroupIndex_.erase(dyingBoss);

    for (auto bit = bossByName_.begin(); bit != bossByName_.end(); ++bit) {
        if (bit->second == dyingBoss) {
            bossByName_.erase(bit);
            break;
        }
    }
}

///========================================================================================
///  事前生成
///========================================================================================
void EnemyManager::PreGenerateEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
    const Vector3& localOffset, const std::string& parentBossName) {

    std::unique_ptr<BaseEnemy> enemy;

    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) {
        enemy = std::make_unique<NormalEnemy>();
        enemy->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
    } else if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)] || enemyType == "BossEnemy") {
        auto strong = std::make_unique<StrongEnemy>();
        strong->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
        strong->SetStrongParameter(strongEnemyParam_);
        enemy = std::move(strong);
    }

    if (!enemy) {
        return;
    }

    enemy->SetPlayer(pPlayer_);
    enemy->SetGameCamera(pGameCamera_);
    enemy->SetManager(this);
    enemy->SetCombo(pCombo_);
    enemy->SetKillCounter(pKillCounter_);
    enemy->SetGroupId(groupID);
    enemy->Init(position);
    enemy->SetHPBarColorConfig(&hpBarColorConfig_);
    UpdateAvailableAnimationsForEditor(enemy.get());

    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetSpawnOffset(localOffset);
        ne->SetNormalParameter(normalEnemyParam_);
    }

    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne2 = static_cast<NormalEnemy*>(enemy.get());
        ne2->SetOnDamageTakenCallback([this, ne2]() {
            if (pDeathTimer_ && ne2->GetZakoState() == NormalEnemy::ZakoState::CrawlBackwards) {
                pDeathTimer_->OnNormalEnemyHit();
            }
        });
    }

    if (enemy->GetType() == BaseEnemy::Type::STRONG && !parentBossName.empty()) {
        bossByName_[parentBossName] = enemy.get();
    }

    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        static_cast<NormalEnemy*>(enemy.get())->SetParentBossName(parentBossName);
    }

    enemy->SetScale(Vector3::ZeroVector());
    enemy->SetAnimationActive(false);
    enemy->SetIsAdaptCollision(false);

    waitingEnemies_[groupID].push_back(std::move(enemy));
}

///========================================================================================
///  待機中の敵を1体アクティブ化
///========================================================================================
bool EnemyManager::ActivateSingleWaitingEnemy(int32_t groupID) {
    auto it = waitingEnemies_.find(groupID);
    if (it == waitingEnemies_.end() || it->second.empty()) {
        return false;
    }

    auto& enemy = it->second.front();

    if (auto* anim = enemy->GetAnimationObject()) {
        anim->ClearAllAnimationEndCallbacks();
    }

    // Refresh parameters so EnemySpawn reads the correct baseScale_
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        enemy->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
    } else if (enemy->GetType() == BaseEnemy::Type::STRONG) {
        enemy->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
        static_cast<StrongEnemy*>(enemy.get())->SetStrongParameter(strongEnemyParam_);
    }

    enemy->SetAnimationActive(true);
    enemy->SetIsAdaptCollision(true);
    enemy->ChangeBehavior(std::make_unique<EnemySpawn>(enemy.get()));

    NormalEnemy* newMinion = nullptr;
    std::string bossName   = "";

    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        newMinion = static_cast<NormalEnemy*>(enemy.get());
        bossName  = newMinion->GetParentBossName();
    }

    enemies_.push_back(std::move(enemy));

    BaseEnemy* activated = enemies_.back().get();
    if (activated->GetType() == BaseEnemy::Type::STRONG) {
        for (auto& [name, ptr] : bossByName_) {
            if (ptr == activated) {
                bossByName_[name] = activated;
                break;
            }
        }
    }

    it->second.erase(it->second.begin());
    if (it->second.empty()) {
        waitingEnemies_.erase(it);
    }

    LinkBossAndMinions(groupID, newMinion, bossName);
    return true;
}

///========================================================================================
///  全待機敵を破棄
///========================================================================================
void EnemyManager::ClearAllWaitingEnemies() {
    for (auto& [groupID, waitingList] : waitingEnemies_) {
        for (auto& e : waitingList) {
            if (e->GetType() == BaseEnemy::Type::STRONG) {
                for (auto bit = bossByName_.begin(); bit != bossByName_.end();) {
                    if (bit->second == e.get()) {
                        bit = bossByName_.erase(bit);
                    } else {
                        ++bit;
                    }
                }
            }
        }
    }
    waitingEnemies_.clear();
}

///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {

    UpdateTauntState();

    for (size_t i = 0; i < enemies_.size();) {

        if (enemies_[i]->GetType() == BaseEnemy::Type::NORMAL) {
            enemies_[i]->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
        } else if (enemies_[i]->GetType() == BaseEnemy::Type::STRONG) {
            enemies_[i]->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
            // StrongParameterも毎フレーム同期
            static_cast<StrongEnemy*>(enemies_[i].get())->SetStrongParameter(strongEnemyParam_);
        }

        enemies_[i]->Update();

        if (enemies_[i]->GetIsDeath()) {
            int32_t groupID = enemies_[i]->GetGroupId();

            if (enemies_[i]->GetType() == BaseEnemy::Type::STRONG) {
                OnBossKilled(enemies_[i].get());
            }

            if (pEnemySpawner_) {
                pEnemySpawner_->OnEnemyDestroyed(groupID);
            }

            if (enemies_[i]->GetType() == BaseEnemy::Type::NORMAL) {
                NormalEnemy* dying = static_cast<NormalEnemy*>(enemies_[i].get());
                BaseEnemy* boss    = dying->GetBoss();
                if (boss) {
                    auto mit = minionsByBoss_.find(boss);
                    if (mit != minionsByBoss_.end()) {
                        auto& minions = mit->second;
                        minions.erase(
                            std::remove(minions.begin(), minions.end(), dying),
                            minions.end());
                    }
                }
            }

            enemies_[i] = std::move(enemies_.back());
            enemies_.pop_back();

            CheckIsEnemiesCleared();
        } else {
            ++i;
        }
    }
}

void EnemyManager::UIUpdate(const KetaEngine::ViewProjection& viewProjection) {
    Vector3 playerPos = pPlayer_->GetBaseTransform().GetWorldPos();
    float radiusSq    = uiOcclusionRadius_ * uiOcclusionRadius_;

    for (size_t i = 0; i < enemies_.size(); ++i) {
        if (enemies_[i]->GetIsDeath()) {
            continue;
        }

        Vector3 enemyPos = enemies_[i]->GetWorldPosition();
        float distance   = (enemyPos - playerPos).Length();

        //  StrongEnemyがカメラとNormalEnemyのUIの間に重なるか
        bool idDraw   = false;
        bool isNormalTarget = enemies_[i]->GetType() == BaseEnemy::Type::NORMAL;
        Vector3 targetNdc = ScreenTransformWithDepth(enemyPos, viewProjection);
        if (isNormalTarget && targetNdc.z > 0.0f && targetNdc.z <= 1.0f) {
            for (size_t j = 0; j < enemies_.size(); ++j) {
                if (i == j || enemies_[j]->GetIsDeath()) {
                    continue;
                }
                if (enemies_[j]->GetType() != BaseEnemy::Type::STRONG) {
                    continue;
                }
                Vector3 occNdc = ScreenTransformWithDepth(enemies_[j]->GetWorldPosition(), viewProjection);
                if (occNdc.z <= 0.0f || occNdc.z >= targetNdc.z) {
                    continue;
                }
                float distanceX = targetNdc.x - occNdc.x;
                float distanceY = targetNdc.y - occNdc.y;
                if (distanceX * distanceX + distanceY * distanceY <= radiusSq) {
                    idDraw = true;
                    break;
                }
            }
        }

        enemies_[i]->DisplaySprite(viewProjection, distance, idDraw);
    }
}

void EnemyManager::DamageReactionCreate() {
    damageReactionController_->EditorUpdate();
}

///========================================================================================
///  パラメータ登録
///========================================================================================
void EnemyManager::RegisterParams() {

    globalParameter_->Regist(groupName_, "hpBarDisplayDistance", &hpBarDisplayDistance_);
    globalParameter_->Regist(groupName_, "bossPlayerTriggerDistance", &bossPlayerTriggerDistance_);
    globalParameter_->Regist(groupName_, "uiOcclusionRadius", &uiOcclusionRadius_);

    for (uint32_t i = 0; i < parameters_.size(); ++i) {
        const std::string& indexString = std::to_string(static_cast<int>(i + 1));

        globalParameter_->Regist(groupName_, "basePosY_" + indexString, &parameters_[i].basePosY);
        globalParameter_->Regist(groupName_, "burstTime" + indexString, &parameters_[i].burstTime);
        globalParameter_->Regist(groupName_, "initScale" + indexString, &parameters_[i].baseScale_);
        globalParameter_->Regist(groupName_, "collisionRad" + indexString, &parameters_[i].collisionRad);
        globalParameter_->Regist(groupName_, "collisionOffset" + indexString, &parameters_[i].collisionOffset);
        globalParameter_->Regist(groupName_, "hpMax" + indexString, &parameters_[i].hpMax);
        globalParameter_->Regist(groupName_, "hpBarPosOffset" + indexString, &parameters_[i].hpBarPosOffset);
        globalParameter_->Regist(groupName_, "hpBarWorldOffsetY" + indexString, &parameters_[i].hpBarWorldOffsetY);
        globalParameter_->Regist(groupName_, "groupIconWorldOffsetY" + indexString, &parameters_[i].groupIconWorldOffsetY);

        globalParameter_->Regist(groupName_, "chaseSpeed" + indexString, &parameters_[i].chaseSpeed);
        globalParameter_->Regist(groupName_, "chaseDistance" + indexString, &parameters_[i].chaseDistance);
        globalParameter_->Regist(groupName_, "waitCooldownTime" + indexString, &parameters_[i].waitCooldownTime);

        globalParameter_->Regist(groupName_, "deathBlowValue" + indexString, &parameters_[i].deathBlowValue);
        globalParameter_->Regist(groupName_, "deathBlowValueY" + indexString, &parameters_[i].deathBlowValueY);
        globalParameter_->Regist(groupName_, "deathGravity" + indexString, &parameters_[i].deathGravity);
        globalParameter_->Regist(groupName_, "deathRotateSpeed" + indexString, &parameters_[i].deathRotateSpeed);
        globalParameter_->Regist(groupName_, "deathBurstTime" + indexString, &parameters_[i].deathBurstTime);
        globalParameter_->Regist(groupName_, "gaugeIncreaseValue" + indexString, &parameters_[i].gaugeIncreaseValue);
    }

    // NormalEnemy専用パラメータ
    globalParameter_->Regist(groupName_, "normal_fleeSpeed", &normalEnemyParam_.fleeSpeed);
    globalParameter_->Regist(groupName_, "normal_fleeTime", &normalEnemyParam_.fleeTime);

    // StrongEnemy専用パラメータ
    globalParameter_->Regist(groupName_, "strong_tauntRange", &strongEnemyParam_.tauntRange);
}

void EnemyManager::DrawEnemyParamUI(BaseEnemy::Type type) {
    const size_t typeIndex = static_cast<size_t>(type);

    ImGui::SeparatorText("基本パラメータ");
    ImGui::DragFloat3("initScale", &parameters_[typeIndex].baseScale_.x, 0.01f);
    ImGui::DragFloat("collisionRad", &parameters_[typeIndex].collisionRad, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat3("collisionOffset", &parameters_[typeIndex].collisionOffset.x, 0.1f);
    ImGui::DragFloat("basePosY", &parameters_[typeIndex].basePosY, 0.01f);
    ImGui::DragFloat("burstTime", &parameters_[typeIndex].burstTime, 0.01f);

    ImGui::SeparatorText("追跡パラメータ");
    ImGui::DragFloat("追跡開始距離", &parameters_[typeIndex].chaseDistance, 0.01f, 0.0f);
    ImGui::DragFloat("追跡速度", &parameters_[typeIndex].chaseSpeed, 0.01f, 0.0f);
    ImGui::DragFloat("Wait後追跡クールダウン(秒)", &parameters_[typeIndex].waitCooldownTime, 0.1f, 0.0f);

    if (type == BaseEnemy::Type::NORMAL) {
        ImGui::SeparatorText("逃走パラメータ（NormalEnemy）");
        ImGui::DragFloat("逃走速度", &normalEnemyParam_.fleeSpeed, 0.1f, 0.0f, 20.0f);
        ImGui::DragFloat("逃走時間", &normalEnemyParam_.fleeTime, 0.1f, 0.0f, 60.0f);
    }

    if (type == BaseEnemy::Type::STRONG) {
        ImGui::SeparatorText("煽りパラメータ（StrongEnemy）");
        ImGui::DragFloat("煽り継続範囲", &strongEnemyParam_.tauntRange, 0.5f, 0.0f, 100.0f);
    }

    ImGui::SeparatorText("死亡パラメータ");
    ImGui::DragFloat("DeathBlowValue", &parameters_[typeIndex].deathBlowValue, 0.1f);
    ImGui::DragFloat("DeathBlowValueY", &parameters_[typeIndex].deathBlowValueY, 0.1f);
    ImGui::DragFloat("DeathGravity", &parameters_[typeIndex].deathGravity, 0.1f);
    ImGui::DragFloat("DeathRotateSpeed", &parameters_[typeIndex].deathRotateSpeed, 0.01f);
    ImGui::DragFloat("DeathBurstTime", &parameters_[typeIndex].deathBurstTime, 0.01f);

    ImGui::SeparatorText("HPパラメータ");
    ImGui::DragFloat("hpMax", &parameters_[typeIndex].hpMax, 1.0f, 1.0f, 9999.0f);

    ImGui::SeparatorText("スプライト関連");
    ImGui::DragFloat2("HPBarOffsetPos", &parameters_[typeIndex].hpBarPosOffset.x, 0.01f);
    ImGui::DragFloat("HPBarWorldOffsetY", &parameters_[typeIndex].hpBarWorldOffsetY, 0.01f);
    ImGui::DragFloat("GroupIconScreenOffsetY", &parameters_[typeIndex].groupIconWorldOffsetY, 1.0f);
}

void EnemyManager::DebugEnemySpawn() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader("Enemy Spawn")) {
        ImGui::DragFloat3("SpawnPosition", &spawnPosition_.x, 0.1f);

        const char* enemyType[] = {"Normal", "Strong"};
        ImGui::Combo("SpawnType", &selectedEnemyTypeIndex_, enemyType, IM_ARRAYSIZE(enemyType));

        if (ImGui::Button("Spawn Enemy")) {
            SpawnEnemy(enemyTypes_[selectedEnemyTypeIndex_], spawnPosition_, 0);
        }
    }
    ImGui::Separator();
#endif
}

void EnemyManager::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("HPバー設定");
        ImGui::DragFloat("HpBar表示距離", &hpBarDisplayDistance_, 0.5f, 0.0f, 200.0f);
        ImGui::DragFloat("ボス煽りトリガー距離", &bossPlayerTriggerDistance_, 0.5f, 0.0f, 100.0f);
        hpBarColorConfig_.AdjustParam();

        for (size_t i = 0; i < static_cast<size_t>(BaseEnemy::Type::COUNT); ++i) {
            BaseEnemy::Type type = static_cast<BaseEnemy::Type>(i);
            ImGui::SeparatorText(enemyTypes_[i].c_str());
            ImGui::PushID(enemyTypes_[i].c_str());

            DrawEnemyParamUI(type);

            ImGui::PopID();
        }

        ImGui::SeparatorText("ボス・取り巻き状態");
        ImGui::Text("煽りトリガー距離: %.1f", bossPlayerTriggerDistance_);
        if (minionsByBoss_.empty()) {
            ImGui::TextColored({1.0f, 0.4f, 0.4f, 1.0f}, "ボスが未リンク");
        }
       

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
    ImGui::Separator();

#endif
}

///========================================================================================
///  クラスセット
///========================================================================================
void EnemyManager::SetPlayer(Player* player) {
    pPlayer_ = player;
}

void EnemyManager::SetCombo(Combo* combo) {
    pCombo_ = combo;
}

void EnemyManager::SetKillCounter(KillCounter* killCounter) {
    pKillCounter_ = killCounter;
}

void EnemyManager::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void EnemyManager::SetEnemySpawner(EnemySpawner* enemySpawner) {
    pEnemySpawner_ = enemySpawner;
}

void EnemyManager::SetDeathTimer(DeathTimer* deathTimer) {
    pDeathTimer_ = deathTimer;
}

void EnemyManager::UpdateAvailableAnimationsForEditor(BaseEnemy* enemy) {
    if (!enemy) {
        return;
    }

    const auto& animeNames = enemy->GetDamageReactionAnimationNames();
    if (!animeNames.empty()) {
        damageReactionController_->SetAvailableAnimations(animeNames);
    }
}

void EnemyManager::CheckIsEnemiesCleared() {
    areAllEnemiesCleared_ = enemies_.empty();
}

bool EnemyManager::IsAnyEnemyInAnticipation() const {
    for (const auto& enemy : enemies_) {
        if (enemy->IsInAnticipation()) {
            return true;
        }
    }
    return false;
}