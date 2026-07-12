#include "EnemyParameter.h"
#include <cstring>
#include <format>
#include <imgui.h>

void EnemyParameter::Init(KetaEngine::GlobalParameter* globalParameter, const std::string& groupName) {
    // グループ作成、パラメータ登録
    globalParameter_ = globalParameter;
    groupName_       = groupName;
    RegisterParams();
}

bool EnemyParameter::CheckDirty() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    // パラメータの変更を検知
    bool dirty = parametersDirty_ ||
        memcmp(parameters_.data(), cachedParameters_.data(), sizeof(parameters_)) != 0 ||
        memcmp(&EntourageEnemyParam_, &cachedEntourageEnemyParam_, sizeof(EntourageEnemyParam_)) != 0 ||
        memcmp(&LeaderEnemyParam_, &cachedLeaderEnemyParam_, sizeof(LeaderEnemyParam_)) != 0;

    // 変更があればキャッシュを更新
    if (dirty) {
        cachedParameters_       = parameters_;
        cachedEntourageEnemyParam_ = EntourageEnemyParam_;
        cachedLeaderEnemyParam_ = LeaderEnemyParam_;
        parametersDirty_        = false;
    }
    return dirty;
#else
    return false;
#endif
}

const BaseEnemy::Parameter& EnemyParameter::GetBaseParam(BaseEnemy::Type type) const {
    return parameters_[static_cast<size_t>(type)];
}

void EnemyParameter::RegisterParams() {
    globalParameter_->Regist(groupName_, "hpBarDisplayDistance", &hpBarDisplayDistance_);
    globalParameter_->Regist(groupName_, "uiOcclusionRadius",    &uiOcclusionRadius_);

    for (size_t i = 0; i < parameters_.size(); ++i) {
        const std::string idx = std::format("{}", static_cast<int>(i + 1));

        globalParameter_->Regist(groupName_, "basePosY_" + idx,              &parameters_[i].basePosY);
        globalParameter_->Regist(groupName_, "burstTime" + idx,              &parameters_[i].burstTime);
        globalParameter_->Regist(groupName_, "initScale" + idx,              &parameters_[i].baseScale_);
        globalParameter_->Regist(groupName_, "collisionRad" + idx,           &parameters_[i].collisionRad);
        globalParameter_->Regist(groupName_, "collisionOffset" + idx,        &parameters_[i].collisionOffset);
        globalParameter_->Regist(groupName_, "hpMax" + idx,                  &parameters_[i].hpMax);
        globalParameter_->Regist(groupName_, "hpBarPosOffset" + idx,         &parameters_[i].hpBarPosOffset);
        globalParameter_->Regist(groupName_, "hpGaugePosOffset" + idx,       &parameters_[i].hpGaugePosOffset);
        globalParameter_->Regist(groupName_, "hpBarWorldOffsetY" + idx,      &parameters_[i].hpBarWorldOffsetY);
        globalParameter_->Regist(groupName_, "groupIconWorldOffsetY" + idx,  &parameters_[i].groupIconWorldOffsetY);
        globalParameter_->Regist(groupName_, "chaseSpeed" + idx,             &parameters_[i].chaseSpeed);
        globalParameter_->Regist(groupName_, "chaseDistance" + idx,          &parameters_[i].chaseDistance);
        globalParameter_->Regist(groupName_, "waitCooldownTime" + idx,       &parameters_[i].waitCooldownTime);
        globalParameter_->Regist(groupName_, "deathBlowValue" + idx,         &parameters_[i].deathBlowValue);
        globalParameter_->Regist(groupName_, "deathBlowValueY" + idx,        &parameters_[i].deathBlowValueY);
        globalParameter_->Regist(groupName_, "deathGravity" + idx,           &parameters_[i].deathGravity);
        globalParameter_->Regist(groupName_, "deathRotateSpeed" + idx,       &parameters_[i].deathRotateSpeed);
        globalParameter_->Regist(groupName_, "deathBurstTime" + idx,         &parameters_[i].deathBurstTime);
        globalParameter_->Regist(groupName_, "deathAnimTimeout" + idx,       &parameters_[i].deathAnimTimeout);
        globalParameter_->Regist(groupName_, "gaugeIncreaseValue" + idx,     &parameters_[i].gaugeIncreaseValue);
    }

    globalParameter_->Regist(groupName_, "normal_fleeSpeed",              &EntourageEnemyParam_.fleeSpeed);
    globalParameter_->Regist(groupName_, "normal_fleeDistance",           &EntourageEnemyParam_.fleeDistance);

    globalParameter_->Regist(groupName_, "strong_fleeSpeed",              &LeaderEnemyParam_.fleeSpeed);
    globalParameter_->Regist(groupName_, "strong_fleeDistance",           &LeaderEnemyParam_.fleeDistance);
    globalParameter_->Regist(groupName_, "strong_fleeCooldownTime",       &LeaderEnemyParam_.fleeCooldownTime);
    globalParameter_->Regist(groupName_, "strong_separationDistance",     &LeaderEnemyParam_.separationDistance);
    globalParameter_->Regist(groupName_, "strong_separationStrength",     &LeaderEnemyParam_.separationStrength);
    globalParameter_->Regist(groupName_, "strong_tauntFontOffset",        &LeaderEnemyParam_.tauntFontOffset);
    globalParameter_->Regist(groupName_, "strong_tauntFontRotateSpeed",   &LeaderEnemyParam_.tauntFontRotateSpeed);
}

void EnemyParameter::DrawManagerParamUI() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::SeparatorText("HPバー設定");
    ImGui::DragFloat("HpBar表示距離",    &hpBarDisplayDistance_, 0.5f, 0.0f, 200.0f);
    ImGui::DragFloat("UIオクルージョン半径", &uiOcclusionRadius_,    0.5f, 0.0f, 500.0f);
#endif
}

void EnemyParameter::DrawEnemyParamUI(BaseEnemy::Type type) {
    const size_t i = static_cast<size_t>(type);

    ImGui::SeparatorText("基本パラメータ");
    ImGui::DragFloat3("initScale",       &parameters_[i].baseScale_.x, 0.01f);
    ImGui::DragFloat("collisionRad",     &parameters_[i].collisionRad, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat3("collisionOffset", &parameters_[i].collisionOffset.x, 0.1f);
    ImGui::DragFloat("basePosY",         &parameters_[i].basePosY, 0.01f);
    ImGui::DragFloat("burstTime",        &parameters_[i].burstTime, 0.01f);

    ImGui::SeparatorText("追跡パラメータ");
    ImGui::DragFloat("追跡開始距離",              &parameters_[i].chaseDistance, 0.01f, 0.0f);
    ImGui::DragFloat("追跡速度",                  &parameters_[i].chaseSpeed, 0.01f, 0.0f);
    ImGui::DragFloat("Wait後追跡クールダウン(秒)", &parameters_[i].waitCooldownTime, 0.1f, 0.0f);

    if (type == BaseEnemy::Type::NORMAL) {
        ImGui::SeparatorText("逃走パラメータ（EntourageEnemy）");
        ImGui::DragFloat("逃走速度", &EntourageEnemyParam_.fleeSpeed,    0.1f, 0.0f, 20.0f);
        ImGui::DragFloat("逃走距離", &EntourageEnemyParam_.fleeDistance, 0.5f, 0.0f, 100.0f);
    }

    if (type == BaseEnemy::Type::STRONG) {
        ImGui::SeparatorText("逃走パラメータ（LeaderEnemy）");
        ImGui::DragFloat("逃走速度",               &LeaderEnemyParam_.fleeSpeed,        0.1f, 0.0f, 30.0f);
        ImGui::DragFloat("Taunt遷移距離（境界から）", &LeaderEnemyParam_.fleeDistance,  0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("逃走開始クールタイム(秒)", &LeaderEnemyParam_.fleeCooldownTime, 0.05f, 0.0f, 5.0f);

        ImGui::SeparatorText("分離パラメータ（LeaderEnemy）");
        ImGui::DragFloat("分離距離", &LeaderEnemyParam_.separationDistance, 0.1f, 0.0f, 50.0f);
        ImGui::DragFloat("分離強度", &LeaderEnemyParam_.separationStrength, 0.1f, 0.0f, 20.0f);

        ImGui::SeparatorText("TauntFont");
        ImGui::DragFloat3("FontOffset",      &LeaderEnemyParam_.tauntFontOffset.x,    0.05f);
        ImGui::DragFloat("FontRotateSpeed",  &LeaderEnemyParam_.tauntFontRotateSpeed, 0.05f);
    }

    ImGui::SeparatorText("死亡パラメータ");
    ImGui::DragFloat("DeathBlowValue",   &parameters_[i].deathBlowValue,   0.1f);
    ImGui::DragFloat("DeathBlowValueY",  &parameters_[i].deathBlowValueY,  0.1f);
    ImGui::DragFloat("DeathGravity",     &parameters_[i].deathGravity,     0.1f);
    ImGui::DragFloat("DeathRotateSpeed", &parameters_[i].deathRotateSpeed, 0.01f);
    ImGui::DragFloat("DeathBurstTime",   &parameters_[i].deathBurstTime,   0.01f);
    ImGui::DragFloat("DeathAnimTimeout", &parameters_[i].deathAnimTimeout, 0.05f, 0.0f, 30.0f);

    ImGui::SeparatorText("HPパラメータ");
    ImGui::DragFloat("hpMax", &parameters_[i].hpMax, 1.0f, 1.0f, 9999.0f);

    ImGui::SeparatorText("スプライト関連");
    ImGui::DragFloat2("HPBarOffsetPos",          &parameters_[i].hpBarPosOffset.x,       0.01f);
    ImGui::DragFloat2("HPGaugeOffsetPos",         &parameters_[i].hpGaugePosOffset.x,     0.01f);
    ImGui::DragFloat("HPBarWorldOffsetY",          &parameters_[i].hpBarWorldOffsetY,      0.01f);
    ImGui::DragFloat("GroupIconScreenOffsetY",     &parameters_[i].groupIconWorldOffsetY,  1.0f);
}
