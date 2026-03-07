#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/DissolveEditor/DissolvePlayer.h"
#include "Editor/RibbonTrailEditor/RibbonTrailPlayer.h"

#include "3d/Object3D/Object3d.h"
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
    virtual void Update();
    virtual void AdjustParam() = 0;


    void RegisterParams(); //< パラメータ登録
    void AdjustParamBase(); //< 基本パラメータの調整

    void PlayDissolve(const std::string& name); //< ディゾルブ再生
    void SetInitialDissolveHidden(); //< 初期非表示状態設定

protected:
    void EffectEmit(const std::string& effectName);

protected:
    ///=============================================
    /// private variant
    ///=============================================

    std::unique_ptr<KetaEngine::Object3d> obj3d_;

    /// グローバルパラメータ
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;

    // emitter
    std::unique_ptr<KetaEngine::ParticlePlayer> particlePlayer_;
    Vector3 direction_;
    Vector3 effectFollowPos_;

    // リボントレイル
    KetaEngine::RibbonTrailPlayer trailPlayer_;

    bool isEmit_   = true;
    bool isShadow_ = true;

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
    void SetIsEmit(const bool& isEmit) { isEmit_ = isEmit; }
    void SetIsShadow(const bool& isShadow) { isShadow_ = isShadow; }

    /// トレイルを即座にクリア（攻撃リセット時などに呼ぶ）
    void ClearTrail() { trailPlayer_.StopAndClear(); }

    /// トレイル放出開始（プリセット名でパラメータをロードして開始）
    void StartTrailEmit(const std::string& presetName, const std::string& category = "Player");
    /// トレイル放出停止
    void StopTrailEmit() { trailPlayer_.StopAndClear(); }
};