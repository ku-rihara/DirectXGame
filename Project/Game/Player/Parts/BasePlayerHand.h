#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"


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

    virtual void AdjustParam() = 0; //< パラメータの調整

    /// <summary>
    /// ディゾルブ効果の適用
    /// </summary>
    /// <param name="dissolve">ディゾルブ値</param>
    virtual void DissolveAdapt(float dissolve);

    void ParamLoadForImGui(); //< ImGui用パラメータロード
    void AddParamGroup();         //< パラメータグループの追加
    void SetValues();            //< 値の設定
    void ApplyGlobalParameter(); //< グローバルパラメータの適用
    void AdjustParamBase(); //< 基本パラメータの調整
    virtual void SaveAndLoad(); //< セーブ・ロード
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

    bool isEmit_   = true;
    bool isShadow_ = true;

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
};