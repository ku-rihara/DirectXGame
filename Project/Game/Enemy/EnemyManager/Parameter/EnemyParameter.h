#pragma once

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
#include "Enemy/Types/LeaderEnemy.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>

/// <summary>
/// 敵のパラメータを管理するクラス
/// </summary>
class EnemyParameter {
public:
    EnemyParameter()  = default;
    ~EnemyParameter() = default;

    void Init(KetaEngine::GlobalParameter* globalParameter, const std::string& groupName);

    // パラメータ変更検知
    bool CheckDirty();

    // ImGui
    void DrawManagerParamUI();
    void DrawEnemyParamUI(BaseEnemy::Type type);

    const BaseEnemy::Parameter&          GetBaseParam(BaseEnemy::Type type) const;
    const EntourageEnemy::NormalParameter&  GetNormalParam() const { return EntourageEnemyParam_; }
    const LeaderEnemy::StrongParameter&  GetStrongParam() const { return LeaderEnemyParam_; }
    float GetHpBarDisplayDistance() const { return hpBarDisplayDistance_; }
    float GetUiOcclusionRadius() const { return uiOcclusionRadius_; }

private:
    void RegisterParams();

    // globalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;

    // EnemyManager固有パラメータ
    float hpBarDisplayDistance_ = 0.0f;
    float uiOcclusionRadius_    = 80.0f;

    // 敵の基本パラメータ、種類ごとのパラメータ
    std::array<BaseEnemy::Parameter, 2> parameters_;
    EntourageEnemy::NormalParameter EntourageEnemyParam_;
    LeaderEnemy::StrongParameter LeaderEnemyParam_;

#if defined(_DEBUG) || defined(DEVELOPMENT)
    // デバッグ用キャッシュ
    std::array<BaseEnemy::Parameter, 2> cachedParameters_;
    EntourageEnemy::NormalParameter cachedEntourageEnemyParam_;
    LeaderEnemy::StrongParameter cachedLeaderEnemyParam_;
    bool parametersDirty_ = true;
#endif
};
