#pragma once

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/StrongEnemy.h"
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
    const NormalEnemy::NormalParameter&  GetNormalParam() const { return normalEnemyParam_; }
    const StrongEnemy::StrongParameter&  GetStrongParam() const { return strongEnemyParam_; }
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
    NormalEnemy::NormalParameter normalEnemyParam_;
    StrongEnemy::StrongParameter strongEnemyParam_;

#if defined(_DEBUG) || defined(DEVELOPMENT)
    // デバッグ用キャッシュ
    std::array<BaseEnemy::Parameter, 2> cachedParameters_;
    NormalEnemy::NormalParameter cachedNormalEnemyParam_;
    StrongEnemy::StrongParameter cachedStrongEnemyParam_;
    bool parametersDirty_ = true;
#endif
};
