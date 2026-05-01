#include "EnemyManager.h"
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
        enemy = std::make_unique<StrongEnemy>();
        enemy->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
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

    // JSONから渡されたローカルオフセットを設定
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetSpawnOffset(localOffset);
        ne->SetNormalParameter(normalEnemyParam_);
    }

    // ダメージコールバック設定
    BaseEnemy* rawPtr = enemy.get();
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        rawPtr->SetOnDamageTakenCallback([this]() {
            if (pDeathTimer_) {
                pDeathTimer_->OnNormalEnemyHit();
            }
        });
    }

    // ボスの場合はボス名→ポインタのテーブルに登録する
    if (enemy->GetType() == BaseEnemy::Type::STRONG && !parentBossName.empty()) {
        bossByName_[parentBossName] = enemy.get();
    }

    NormalEnemy* newMinion = nullptr;
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        newMinion = static_cast<NormalEnemy*>(enemy.get());
    }

    enemies_.push_back(std::move(enemy));

    // ボス・取り巻きのリンク
    LinkBossAndMinions(groupID, newMinion, parentBossName);
}

///========================================================================================
///  ボス・取り巻きリンク
///========================================================================================
void EnemyManager::LinkBossAndMinions(int32_t groupID, NormalEnemy* newMinion, const std::string& parentBossName) {

    // このウェーブグループに属する全ボスを収集・登録
    for (auto& e : enemies_) {
        if (e->GetGroupId() == groupID && e->GetType() == BaseEnemy::Type::STRONG) {
            if (minionsByBoss_.find(e.get()) == minionsByBoss_.end()) {
                minionsByBoss_[e.get()] = {};
            }
        }
    }

    // 新しく生成されたザコのみをリンク対象とする
    if (!newMinion) {
        return;
    }
    if (newMinion->GetBoss() != nullptr) {
        return;
    } // 既にリンク済み

    BaseEnemy* targetBoss = nullptr;

    //  JSON で parent_boss_name が指定されている場合はボス名で直接引く
    if (!parentBossName.empty()) {
        auto it = bossByName_.find(parentBossName);
        if (it != bossByName_.end()) {
            targetBoss = it->second;
        }
    }

    // parentBossName が空またはボスがまだスポーンしていない場合は
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
    } // ボスがまだ生成されていない

    newMinion->SetBoss(targetBoss);

    // オフセットが未設定の場合のみ現在位置差から計算する
    const Vector3& knownOffset = newMinion->GetSpawnOffset();
    bool hasPresetOffset       = (knownOffset.x != 0.0f || knownOffset.z != 0.0f);
    if (!hasPresetOffset) {
        Vector3 offset = newMinion->GetWorldPosition() - targetBoss->GetWorldPosition();
        offset.y       = 0.0f;
        newMinion->SetSpawnOffset(offset);
    }

    // ミニオンリストに追加
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
    if (pDeathTimer_) {
        pDeathTimer_->OnBossDead();
    }

    // そのボスの取り巻きのみ逃走させる
    auto it = minionsByBoss_.find(dyingBoss);
    if (it != minionsByBoss_.end()) {
        for (NormalEnemy* zako : it->second) {
            if (zako && !zako->GetIsDeath()) {
                zako->StartFlee();
            }
        }
        minionsByBoss_.erase(it);
    }

    //  bossByName_ からも削除する
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
        enemy = std::make_unique<StrongEnemy>();
        enemy->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
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

    // JSONから渡されたローカルオフセットを設定
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetSpawnOffset(localOffset);
        ne->SetNormalParameter(normalEnemyParam_);
    }

    // ダメージコールバック設定
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        enemy->SetOnDamageTakenCallback([this]() {
            if (pDeathTimer_) {
                pDeathTimer_->OnNormalEnemyHit();
            }
        });
    }

    //  事前生成のボスも bossByName_ に登録しておく
    if (enemy->GetType() == BaseEnemy::Type::STRONG && !parentBossName.empty()) {
        // waitingEnemies_ に入る前に raw ポインタを退避して登録
        bossByName_[parentBossName] = enemy.get();
    }

    // parentBossName を NormalEnemy に保存しておく
    if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        static_cast<NormalEnemy*>(enemy.get())->SetParentBossName(parentBossName);
    }

    // 非アクティブ化
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
    enemy->SetAnimationActive(true);
    enemy->SetIsAdaptCollision(true);
    enemy->ChangeBehavior(std::make_unique<EnemySpawn>(enemy.get()));

    // アクティブ化時にボスなら bossByName_ のポインタを正式に更新する
    NormalEnemy* newMinion = nullptr;
    std::string bossName   = "";

    if (enemy->GetType() == BaseEnemy::Type::STRONG) {
        // ボスのアクティブ化：bossByName_ の登録ポインタを更新
    } else if (enemy->GetType() == BaseEnemy::Type::NORMAL) {
        newMinion = static_cast<NormalEnemy*>(enemy.get());
        bossName  = newMinion->GetParentBossName();
    }

    enemies_.push_back(std::move(enemy));

    // move 後の最後尾ポインタで bossByName_ を更新
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

    // ザコのみリンクを試みる
    LinkBossAndMinions(groupID, newMinion, bossName);
    return true;
}

///========================================================================================
///  全待機敵を破棄
///========================================================================================
void EnemyManager::ClearAllWaitingEnemies() {
    //  待機中のボスが bossByName_ に登録されていたら合わせて削除する
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

    // 煽り状態を毎フレーム評価
    UpdateTauntState();

    for (size_t i = 0; i < enemies_.size();) {

        if (enemies_[i]->GetType() == BaseEnemy::Type::NORMAL) {
            enemies_[i]->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
        } else if (enemies_[i]->GetType() == BaseEnemy::Type::STRONG) {
            enemies_[i]->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
        }

        enemies_[i]->Update();

        if (enemies_[i]->GetIsDeath()) {
            int32_t groupID = enemies_[i]->GetGroupId();

            // ボス撃破通知
            if (enemies_[i]->GetType() == BaseEnemy::Type::STRONG) {
                OnBossKilled(enemies_[i].get());
            }

            // スポーナーへ通知
            if (pEnemySpawner_) {
                pEnemySpawner_->OnEnemyDestroyed(groupID);
            }

            // 死亡した取り巻きをミニオンリストから除去
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

void EnemyManager::HpBarUpdate(const KetaEngine::ViewProjection& viewProjection) {
    Vector3 playerPos = pPlayer_ ? pPlayer_->GetBaseTransform().GetWorldPos() : Vector3{};

    for (size_t i = 0; i < enemies_.size(); ++i) {
        Vector3 enemyPos = enemies_[i]->GetWorldPosition();
        float dist       = (enemyPos - playerPos).Length();

        if (dist <= hpBarDisplayDistance_) {
            enemies_[i]->DisplaySprite(viewProjection);
        } else {
            enemies_[i]->HideHpBar();
        }
    }
}

void EnemyManager::DamageReactionCreate() {
    damageReactionController_->EditorUpdate();
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void EnemyManager::RegisterParams() {

    globalParameter_->Regist(groupName_, "hpBarDisplayDistance", &hpBarDisplayDistance_);
    globalParameter_->Regist(groupName_, "bossPlayerTriggerDistance", &bossPlayerTriggerDistance_);

    for (uint32_t i = 0; i < parameters_.size(); ++i) {
        const std::string& indexString = std::to_string(static_cast<int>(i + 1));

        globalParameter_->Regist(groupName_, "basePosY_" + indexString, &parameters_[i].basePosY);
        globalParameter_->Regist(groupName_, "burstTime" + indexString, &parameters_[i].burstTime);
        globalParameter_->Regist(groupName_, "initScale" + indexString, &parameters_[i].baseScale_);
        globalParameter_->Regist(groupName_, "collisionRad" + indexString, &parameters_[i].collisionRad);
        globalParameter_->Regist(groupName_, "collisionOffset" + indexString, &parameters_[i].collisionOffset);
        globalParameter_->Regist(groupName_, "hpBarPosOffset" + indexString, &parameters_[i].hpBarPosOffset);

        globalParameter_->Regist(groupName_, "chaseDistanceMin" + indexString, &parameters_[i].chaseDistanceMin);
        globalParameter_->Regist(groupName_, "chaseSpeed" + indexString, &parameters_[i].chaseSpeed);
        globalParameter_->Regist(groupName_, "chaseDistance" + indexString, &parameters_[i].chaseDistance);

        globalParameter_->Regist(groupName_, "deathBlowValue" + indexString, &parameters_[i].deathBlowValue);
        globalParameter_->Regist(groupName_, "deathBlowValueY" + indexString, &parameters_[i].deathBlowValueY);
        globalParameter_->Regist(groupName_, "deathGravity" + indexString, &parameters_[i].deathGravity);
        globalParameter_->Regist(groupName_, "deathRotateSpeed" + indexString, &parameters_[i].deathRotateSpeed);
        globalParameter_->Regist(groupName_, "deathBurstTime" + indexString, &parameters_[i].deathBurstTime);
        globalParameter_->Regist(groupName_, "gaugeIncreaseValue" + indexString, &parameters_[i].gaugeIncreaseValue);
    }

    // NormalEnemy専用パラメータ
    globalParameter_->Regist(groupName_, "normal_fleeSpeed", &normalEnemyParam_.fleeSpeed);
    globalParameter_->Regist(groupName_, "normal_fleeTime",  &normalEnemyParam_.fleeTime);
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
    ImGui::DragFloat("追跡限度", &parameters_[typeIndex].chaseDistanceMin, 0.1f, 0.0f);
    ImGui::DragFloat("追跡速度", &parameters_[typeIndex].chaseSpeed, 0.01f, 0.0f);

    if (type == BaseEnemy::Type::NORMAL) {
        ImGui::SeparatorText("逃走パラメータ（NormalEnemy）");
        ImGui::DragFloat("逃走速度", &normalEnemyParam_.fleeSpeed, 0.1f, 0.0f, 20.0f);
        ImGui::DragFloat("逃走時間", &normalEnemyParam_.fleeTime,  0.1f, 0.0f, 60.0f);
    }

    ImGui::SeparatorText("死亡パラメータ");
    ImGui::DragFloat("DeathBlowValue", &parameters_[typeIndex].deathBlowValue, 0.1f);
    ImGui::DragFloat("DeathBlowValueY", &parameters_[typeIndex].deathBlowValueY, 0.1f);
    ImGui::DragFloat("DeathGravity", &parameters_[typeIndex].deathGravity, 0.1f);
    ImGui::DragFloat("DeathRotateSpeed", &parameters_[typeIndex].deathRotateSpeed, 0.01f);
    ImGui::DragFloat("DeathBurstTime", &parameters_[typeIndex].deathBurstTime, 0.01f);

    ImGui::SeparatorText("スプライト関連");
    ImGui::DragFloat2("HPBarOffsetPos", &parameters_[typeIndex].hpBarPosOffset.x, 0.01f);
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

        // デバッグ情報
        ImGui::SeparatorText("ボス・取り巻き状態");
        ImGui::Text("煽りトリガー距離: %.1f", bossPlayerTriggerDistance_);
        if (minionsByBoss_.empty()) {
            ImGui::TextColored({1.0f, 0.4f, 0.4f, 1.0f}, "ボスが未リンク");
        }
        Vector3 playerPos = pPlayer_ ? pPlayer_->GetBaseTransform().GetWorldPos() : Vector3{};
        int32_t bossIdx   = 0;
        for (auto& [boss, minions] : minionsByBoss_) {
            bool alive   = boss && !boss->GetIsDeath();
            float dist   = alive ? (boss->GetWorldPosition() - playerPos).Length() : -1.0f;
            int taunting = 0;
            for (auto* m : minions) {
                if (m && m->IsTaunting())
                    ++taunting;
            }
            ImGui::Text("Boss[%d]: alive=%s ToPlayer=%.1f Minions=%d Taunting=%d",
                bossIdx++, alive ? "YES" : "NO", dist, (int)minions.size(), taunting);
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
    ImGui::Separator();

#endif
}

///------------------------------------------------------------------------------------------------
/// クラスセット
///------------------------------------------------------------------------------------------------
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
