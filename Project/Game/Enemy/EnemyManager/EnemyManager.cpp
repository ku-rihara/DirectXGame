#include "EnemyManager.h"
// UIs
#include "Enemy/UIs/GroupIcon/GroupIcon.h"
// Math
#include "Matrix4x4.h"
// Behavior
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
// Player
#include "Player/Player.h"
// StressGauge
#include "StressGauge/StressGauge.h"
// Combo
#include "Combo/Combo.h"
// LockOn
#include "LockOn/LockOn.h"
#include "LockOn/LockOnController.h"
// imGui
#include <imgui.h>

///========================================================================================
///  デストラクタ
///========================================================================================
EnemyManager::~EnemyManager() {
    // アクティブ敵のビヘイビアを先にリセットしてから破棄
    for (auto& e : enemies_) {
        if (e) { e->PrepareForPool(); }
    }
    enemies_.clear();

    // 待機敵のビヘイビアを先にリセットしてから破棄
    for (auto& [id, list] : waitingEnemies_) {
        for (auto& e : list) {
            if (e) { e->PrepareForPool(); }
        }
    }
    waitingEnemies_.clear();

    minionsByBoss_.clear();
    bossByName_.clear();
    bossGroupIndex_.clear();

    // プールと管理クラスを明示的に破棄
    pool_.reset();
    damageReactionController_.reset();
}

///========================================================================================
///  初期化
///========================================================================================
void EnemyManager::Init() {

    // デバッグ用スポーン位置と敵タイプの初期化
    selectedEnemyType_ = "EntourageEnemy";
    spawnPosition_     = {};

    // 他クラスのポインタ取得
    damageReactionController_ = std::make_unique<EnemyDamageReactionController>();
    damageReactionController_->Init();

    // グローバルパラメータの取得
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);

    // 敵パラメータ
    param_ = std::make_unique<EnemyParameter>();
    param_->Init(globalParameter_, groupName_);

    // ボムパラメータ（一度だけ初期化）
    bombParam_ = std::make_unique<BossAttackBombParameter>();
    bombParam_->Init();

    globalParameter_->SyncParamForGroup(groupName_);

    // HPバーの色設定
    hpBarColorConfig_.Init();

    // オブジェクトプール事前確保
    pool_ = std::make_unique<EnemyPool>();
    pool_->Init();
}

///========================================================================================
///  EnemySpawner生成・初期化
///========================================================================================
void EnemyManager::InitSpawner(const std::string& jsonFile) {
    // 生成して初期化
    spawner_ = std::make_unique<EnemySpawner>();
    spawner_->Init(jsonFile, this);
}

///========================================================================================
///  SpawnEnemy
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
    const Vector3& localOffset, const std::string& parentBossName) {
    if (spawner_) {
        spawner_->SpawnEnemy(enemyType, position, groupID, localOffset, parentBossName);
    }
}

///========================================================================================
///  プールからの取得
///========================================================================================
std::unique_ptr<BaseEnemy> EnemyManager::AcquireFromPool(BaseEnemy::Type type) {
    return pool_->Acquire(type);
}

///========================================================================================
///  activeリストへ登録
///========================================================================================
void EnemyManager::RegisterEnemy(std::unique_ptr<BaseEnemy> enemy, int32_t groupID, const std::string& bossName) {
    if (enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG && !bossName.empty()) {
        bossByName_[bossName] = enemy.get();
    }

    EntourageEnemy* newMinion = nullptr;
    if (enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::NORMAL) {
        newMinion = static_cast<EntourageEnemy*>(enemy.get());
    }

    enemies_.push_back(std::move(enemy));
    LinkBossAndMinions(groupID, newMinion, bossName);
}

///========================================================================================
///  waitingリストへ登録
///========================================================================================
void EnemyManager::RegisterWaitingEnemy(std::unique_ptr<BaseEnemy> enemy, int32_t groupID, const std::string& bossName) {
    if (enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG && !bossName.empty()) {
        bossByName_[bossName] = enemy.get();
    }
    waitingEnemies_[groupID].push_back(std::move(enemy));
}

///========================================================================================
///  EntourageEnemyダメージコールバック
///========================================================================================
void EnemyManager::OnEntourageEnemyDamaged(EntourageEnemy* ne) {
    if (pStressGauge_ && ne->GetZakoState() == EntourageEnemy::ZakoState::CrawlBackwards) {
        pStressGauge_->OnEntourageEnemyHit();
    }
}

///========================================================================================
///  ボス・取り巻きリンク
///========================================================================================
void EnemyManager::LinkBossAndMinions(int32_t groupID, EntourageEnemy* newMinion, const std::string& parentBossName) {

    for (auto& e : enemies_) {
        if (e->GetGroupId() == groupID && e->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG) {
            if (minionsByBoss_.find(e.get()) == minionsByBoss_.end()) {
                minionsByBoss_[e.get()]  = {};
                bossGroupIndex_[e.get()] = nextBossGroupIndex_++;
            }
        }
    }

    if (!newMinion || newMinion->GetBoss() != nullptr) {
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
            if (enemy->GetGroupId() == groupID && enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG) {
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
///  煽り状態更新
///========================================================================================
void EnemyManager::UpdateTauntState() {
    if (!pPlayer_ || !pStressGauge_) {
        return;
    }

    for (auto& [boss, minions] : minionsByBoss_) {
        if (!boss || boss->GetIsDeath()) {
            continue;
        }

        bool bossIsTaunting = false;
        if (LeaderEnemy* strong = dynamic_cast<LeaderEnemy*>(boss)) {
            bossIsTaunting = strong->IsTaunting();
        }

        for (EntourageEnemy* zako : minions) {
            if (!zako || zako->GetIsDeath()) {
                continue;
            }

            if (bossIsTaunting) {
                zako->StartTaunt();
            } else {
                zako->StopTaunt();
            }
        }
    }
}

///========================================================================================
///  ボス撃破処理
///========================================================================================
void EnemyManager::OnBossKilled(BaseEnemy* dyingBoss) {

    auto it = minionsByBoss_.find(dyingBoss);
    if (it != minionsByBoss_.end()) {
        for (EntourageEnemy* zako : it->second) {
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
///  待機中の敵を1体アクティブ化
///========================================================================================
bool EnemyManager::ActivateSingleWaitingEnemy(int32_t groupID) {
    auto it = waitingEnemies_.find(groupID);
    if (it == waitingEnemies_.end() || it->second.empty()) {
        return false;
    }

    auto& enemy = it->second.front();

    if (auto* anim = enemy->GetAnimator()->GetAnimationObject()) {
        anim->ClearAllAnimationEndCallbacks();
    }

    BaseEnemy::Type t = enemy->GetBaseInfo()->GetType();
    enemy->GetBaseInfo()->SetParameter(t, param_->GetBaseParam(t));
    if (t == BaseEnemy::Type::STRONG) {
        static_cast<LeaderEnemy*>(enemy.get())->SetStrongParameter(param_->GetStrongParam());
    }
    enemy->RefreshCollision();

    enemy->GetAnimator()->SetAnimationActive(true);
    enemy->ChangeBehavior(std::make_unique<EnemySpawn>(enemy.get()));

    EntourageEnemy* newMinion = nullptr;
    std::string bossName   = "";

    if (t == BaseEnemy::Type::NORMAL) {
        newMinion = static_cast<EntourageEnemy*>(enemy.get());
        bossName  = newMinion->GetParentBossName();
    }

    enemies_.push_back(std::move(enemy));

    BaseEnemy* activated = enemies_.back().get();
    if (activated->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG) {
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
            if (e->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG) {
                for (auto bit = bossByName_.begin(); bit != bossByName_.end();) {
                    if (bit->second == e.get()) {
                        bit = bossByName_.erase(bit);
                    } else {
                        ++bit;
                    }
                }
            }
            e->PrepareForPool();
            pool_->Release(std::move(e));
        }
    }
    waitingEnemies_.clear();
}

///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {

    UpdateTauntState();

#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (param_->CheckDirty()) {
        for (auto& enemy : enemies_) {
            BaseEnemy::Type t = enemy->GetBaseInfo()->GetType();
            enemy->GetBaseInfo()->SetParameter(t, param_->GetBaseParam(t));
            enemy->RefreshCollision();
            if (t == BaseEnemy::Type::STRONG) {
                static_cast<LeaderEnemy*>(enemy.get())->SetStrongParameter(param_->GetStrongParam());
            }
        }
    }
#endif

    for (size_t i = 0; i < enemies_.size();) {

        enemies_[i]->Update();

        if (enemies_[i]->GetIsDeath()) {
            int32_t groupID = enemies_[i]->GetGroupId();

            if (enemies_[i]->GetBaseInfo()->GetType() == BaseEnemy::Type::STRONG) {
                OnBossKilled(enemies_[i].get());
                if (spawner_) {
                    spawner_->OnLeaderEnemyDestroyed(groupID);
                }
            }

            if (pPlayer_ && pPlayer_->GetContext().pLockOn) {
                pPlayer_->GetContext().pLockOn->GetLockOn()->OnObjectDestroyed(enemies_[i].get());
            }

            if (spawner_) {
                spawner_->OnEnemyDestroyed(groupID);
            }

            if (enemies_[i]->GetBaseInfo()->GetType() == BaseEnemy::Type::NORMAL) {
                EntourageEnemy* dying = static_cast<EntourageEnemy*>(enemies_[i].get());
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

            auto dying = std::move(enemies_[i]);
            dying->PrepareForPool();
            pool_->Release(std::move(dying));

            enemies_[i] = std::move(enemies_.back());
            enemies_.pop_back();

            CheckIsEnemiesCleared();
        } else {
            ++i;
        }
    }
}

void EnemyManager::UIUpdate(const KetaEngine::ViewProjection& viewProjection) {
    Vector3 playerPos   = pPlayer_->GetBaseTransform().GetWorldPos();
    float occlusionRad  = param_->GetUiOcclusionRadius();
    float radiusSq      = occlusionRad * occlusionRad;

    for (size_t i = 0; i < enemies_.size(); ++i) {
        if (enemies_[i]->GetIsDeath()) {
            continue;
        }

        Vector3 enemyPos = enemies_[i]->GetWorldPosition();
        float distance   = (enemyPos - playerPos).Length();

        bool idDraw         = false;
        bool isNormalTarget = enemies_[i]->GetBaseInfo()->GetType() == BaseEnemy::Type::NORMAL;
        Vector3 targetNdc   = ScreenTransformWithDepth(enemyPos, viewProjection);
        if (isNormalTarget && targetNdc.z > 0.0f && targetNdc.z <= 1.0f) {
            for (size_t j = 0; j < enemies_.size(); ++j) {
                if (i == j || enemies_[j]->GetIsDeath()) {
                    continue;
                }
                if (enemies_[j]->GetBaseInfo()->GetType() != BaseEnemy::Type::STRONG) {
                    continue;
                }
                Vector3 occNdc = ScreenTransformWithDepth(enemies_[j]->GetWorldPosition(), viewProjection);
                if (occNdc.z <= 0.0f || occNdc.z >= targetNdc.z) {
                    continue;
                }
                float dx = targetNdc.x - occNdc.x;
                float dy = targetNdc.y - occNdc.y;
                if (dx * dx + dy * dy <= radiusSq) {
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
///  パラメータ調整（ImGui）
///========================================================================================
void EnemyManager::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        param_->DrawManagerParamUI();
        hpBarColorConfig_.AdjustParam();

        for (size_t i = 0; i < static_cast<size_t>(BaseEnemy::Type::COUNT); ++i) {
            BaseEnemy::Type type = static_cast<BaseEnemy::Type>(i);
            ImGui::SeparatorText(enemyTypes_[i].c_str());
            ImGui::PushID(enemyTypes_[i].c_str());
            param_->DrawEnemyParamUI(type);
            ImGui::PopID();
        }

        ImGui::SeparatorText("ボス・取り巻き状態");
        if (minionsByBoss_.empty()) {
            ImGui::TextColored({1.0f, 0.4f, 0.4f, 1.0f}, "ボスが未リンク");
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
    ImGui::Separator();

    if (spawner_) {
        spawner_->AdjustParam();
    }

    if (bombParam_) {
        bombParam_->AdjustParam();
    }
#endif
}

void EnemyManager::DebugEnemySpawn() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader("Enemy Spawn")) {
        ImGui::DragFloat3("SpawnPosition", &spawnPosition_.x, 0.1f);

        const char* enemyType[] = {"Normal", "Strong"};
        ImGui::Combo("SpawnType", &selectedEnemyTypeIndex_, enemyType, IM_ARRAYSIZE(enemyType));

        if (ImGui::Button("Spawn Enemy")) {
            if (spawner_) {
                spawner_->SpawnEnemy(enemyTypes_[selectedEnemyTypeIndex_], spawnPosition_, 0);
            }
        }
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

void EnemyManager::SetStressGauge(StressGauge* StressGauge) {
    pStressGauge_ = StressGauge;
}

void EnemyManager::UpdateAvailableAnimationsForEditor(BaseEnemy* enemy) {
    if (!enemy) {
        return;
    }

    const auto& animeNames = enemy->GetAnimator()->GetDamageReactionAnimationNames();
    if (!animeNames.empty()) {
        int typeIndex = static_cast<int>(enemy->GetBaseInfo()->GetType());
        damageReactionController_->SetAvailableAnimations(typeIndex, animeNames);
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
