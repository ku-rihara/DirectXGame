#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "utility/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
#include "Material/ModelMaterial.h"
#include "Model.h"
#include "ObjectColor.h"
#include "ShadowMap/ShadowMap.h"
#include "struct/TransformationMatrix.h"
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// 3Dオブジェクトの基底クラス
/// </summary>
class BaseObject3d {
public:
    ///========================================================================================
    ///  public method
    ///========================================================================================

    BaseObject3d()          = default;
    virtual ~BaseObject3d() = default;

    virtual void CreateMaterialResource(); //< マテリアルのリソース作成
    virtual void CreateShadowMap(); //< シャドウマップ作成
    virtual void CreateWVPResource(); //< WVPリソース作成
    virtual void DebugImgui(); //< ImGuiデバッグ表示

    /// <summary>
    /// WVPデータの更新
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション行列</param>
    virtual void UpdateWVPData(const ViewProjection& viewProjection);

    /// <summary>
    /// テクスチャのセット
    /// </summary>
    /// <param name="name">テクスチャ名</param>
    void SetTexture(const std::string& name);

    /// <summary>
    /// モデル名からモデルをセット
    /// </summary>
    /// <param name="modelName">モデル名</param>
    void SetModel(const std::string& modelName);

    /// <summary>
    /// オブジェクトイージングアニメーション再生
    /// </summary>
    /// <param name="categoryName">カテゴリー名</param>
    /// <param name="animationName">アニメーション名</param>
    void PlayObjEaseAnimation(const std::string& categoryName, const std::string& animationName);

    /// <summary>
    /// オブジェクトイージングアニメーション停止
    /// </summary>
    void StopObjEaseAnimation();

    /// <summary>
    /// アニメーション更新（継承先のUpdate関数内で呼び出す）
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateObjEaseAnimation(const float& deltaTime);

protected:
    /// <summary>
    /// アニメーション適用後のTransform更新
    /// </summary>
    void ApplyAnimationToTransform();

public:
    ModelMaterial material_; //< モデルのマテリアル
    ObjectColor objColor_; //< オブジェクトの色
    WorldTransform transform_; //< ワールド変換

protected:
    ///========================================================================================
    ///  protected variant
    ///========================================================================================

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_; //< WVPリソース
    TransformationMatrix* wvpDate_;                      //< WVPデータのポインタ
    ShadowMap* shadowMap_;                               //< シャドウマップ

    bool isShadow_ = true;  //< 影を描画するか
    bool isDraw_   = true;  //< 描画するか
    uint32_t textureIndex_; //< テクスチャインデックス

    Model* model_       = nullptr;         //< モデルデータ
    BlendMode blendMode = BlendMode::None; //< ブレンドモード

    const std::string textureFirePath_ = "Resources/Texture/ModelTexture/"; 

    // オブジェクトイージングアニメーション
    std::unique_ptr<ObjEaseAnimationPlayer> objEaseAnimationPlayer_;

public:
    ///========================================================================================
    ///  getter method
    ///========================================================================================
    Model* GetModel() { return model_; }
    const int32_t& GetTextureIndex() const { return textureIndex_; }
    ObjEaseAnimationPlayer* GetObjEaseAnimationPlayer() { return objEaseAnimationPlayer_.get(); }

    ///========================================================================================
    ///  setter method
    ///========================================================================================
    void SetIsDraw(const bool& is) { isDraw_ = is; }
    void SetModel(Model* model) { model_ = model; }
    void SetBlendMode(BlendMode mode) { blendMode = mode; }
    void SetwvpDate(const Matrix4x4& date) { wvpDate_->WVP = date; }
    void SetWorldMatrixDate(const Matrix4x4& date) { wvpDate_->World = date; }
    void SetIsShadow(const bool& is) { isShadow_ = is; }
};