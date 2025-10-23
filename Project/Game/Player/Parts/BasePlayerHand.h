#pragma once

#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/RailEditor/ControlPosManager.h"
#include "utility/RailEditor/RailManager.h"

#include "BaseObject/BaseObject.h"
#include "utility/ParticleEditor/ParticleEmitter.h"

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
    virtual void DissolveAdapt(const float& dissolve);

    void SetBlendModeSub(); //< ブレンドモードをSubに設定
    void SetBlendModeAdd(); //< ブレンドモードをAddに設定

    void ParamLoadForImGui();    //< ImGui用パラメータロード
    void AddParamGroup();        //< パラメータグループの追加
    void SetValues();            //< 値の設定
    void ApplyGlobalParameter(); //< グローバルパラメータの適用
    void AdjustParamBase();      //< 基本パラメータの調整
    virtual void SaveAndLoad();  //< セーブ・ロード

protected:
    ///=============================================
    /// private variant
    ///=============================================

    /// グローバルパラメータ
    GlobalParameter* globalParameter_;
    std::string groupName_;

    // emitter
    std::unique_ptr<ParticleEmitter> emitter_;
    Vector3 direction_;

    /// parameter
    float railRunSpeedThree_;
    float railRunSpeedForth_;

    bool isEmit_   = true;
    bool isShadow_ = true;

public:
    ///========================================================
    /// getter method
    ///========================================================
    Vector3 GetDirection() const { return direction_; }
    const WorldTransform& GetObjTransform() const { return obj3d_->transform_; }
    float GetRailRunSpeedThree() const { return railRunSpeedThree_; }
    float GetRailRunSpeedForth() const { return railRunSpeedForth_; }

    ///========================================================
    /// setter method
    ///========================================================
    virtual void SetRailParent(WorldTransform* parent) = 0;
    virtual void SetParent(WorldTransform* parent);
    void SetObjTranslate(const Vector3& pos) { obj3d_->transform_.translation_ = pos; }
    void SetIsEmit(const bool& isEmit) { isEmit_ = isEmit; }
    void SetIsShadow(const bool& isShadow) { isShadow_ = isShadow; }
};