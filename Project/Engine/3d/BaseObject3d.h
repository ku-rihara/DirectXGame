#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "Material/ModelMaterial.h"
#include "Model.h"
#include "ObjectColor.h"
#include "ShadowMap/ShadowMap.h"
#include "struct/TransformationMatrix.h"
#include <cstdint>
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

public:
    ModelMaterial material_; //< モデルのマテリアル
    ObjectColor objColor_;   //< オブジェクトの色
    WorldTransform transform_; //< ワールド変換

protected:
    ///========================================================================================
    ///  protected variant
    ///========================================================================================

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_; //< WVPリソース
    TransformationMatrix* wvpDate_;                      //< WVPデータのポインタ
    ShadowMap* shadowMap_;                               //< シャドウマップ

    bool isShadow_ = true;   //< 影を描画するか
    bool isDraw_   = true;   //< 描画するか
    uint32_t textureIndex_;  //< テクスチャインデックス

    Model* model_       = nullptr;          //< モデルデータ
    BlendMode blendMode = BlendMode::None;  //< ブレンドモード

    const std::string textureFirePath_ = "Resources/Texture/ModelTexture/"; //< テクスチャファイルパス

public:
    ///========================================================================================
    ///  getter method
    ///========================================================================================
    Model* GetModel() { return model_; }
    const int32_t& GetTextureIndex() const { return textureIndex_; }

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