#pragma once

#include "Editor/DissolveEditor/DissolvePlayer.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RibbonTrailEditor/RibbonTrailPlayer.h"

#include "3D/Object3D/Object3d.h"
#include "BaseObject/BaseObject.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include <memory>
#include <string>

/// <summary>
/// プレイヤーの手の基底クラス
/// </summary>
class BasePlayerHand : public BaseObject {
public:
    virtual ~BasePlayerHand() = default;
    ///============================================
    /// public method
    ///=============================================

    // 初期化、更新
    virtual void Init();
    virtual void Update(bool isDeath);
    virtual void AdjustParam() = 0;

    // パラメータ登録、調節
    void RegisterParams();
    void AdjustParamBase();

    // ディゾルブ再生、初期の非表示状態をセット
    void PlayDissolve(const std::string& name);
    void SetInitialDissolveHidden();

    /// トレイルクリア、放出開始、停止
    void ClearTrail() { trailPlayer_.StopAndClear(); }
    void StartTrailEmit(const std::string& presetName, const std::string& category = "Player");
    void StopTrailEmit() { trailPlayer_.StopEmit(); }
 
protected:
    /// <summary>
    /// エフェクト発射
    /// </summary>
    /// <param name="effectName">エフェクト名</param>
    void EffectEmit(const std::string& effectName);

private:
    /// <summary>
    /// 死亡フラグによる更新処理
    /// </summary>
    /// <param name="isDeath">死亡フラグ</param>
    void UpdateByDeathFrag(bool isDeath);
protected:
    ///=============================================
    /// protected variables
    ///=============================================

    std::unique_ptr<KetaEngine::Object3d> obj3d_;

    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;

    // エフェクト放出
    std::unique_ptr<KetaEngine::ParticlePlayer> particlePlayer_;
    Vector3 direction_;          ///< エフェクト放出方向
    Vector3 effectFollowPos_;    ///< エフェクトの追従座標

    // リボントレイル
    KetaEngine::RibbonTrailPlayer trailPlayer_;

    bool isEmit_   = true;  ///< エフェクト放出フラグ
    bool isShadow_ = true;  ///< 影描画フラグ

    KetaEngine::DissolvePlayer dissolvePlayer_;

public:
    ///========================================================
    /// getter method
    ///========================================================
    Vector3 GetDirection() const { return direction_; }
    const KetaEngine::WorldTransform& GetObjTransform() const { return obj3d_->transform_; }
    KetaEngine::Object3d* GetObject3D() const { return obj3d_.get(); }
    ///========================================================
    /// setter method
    ///========================================================
    virtual void SetParent(KetaEngine::WorldTransform* parent);
    void SetObjTranslate(const Vector3& pos) { obj3d_->transform_.translation_ = pos; }
    void SetIsEmit(bool isEmit) { isEmit_ = isEmit; }
    void SetIsShadow(bool isShadow) { isShadow_ = isShadow; }
};