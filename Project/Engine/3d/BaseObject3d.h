#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
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
    virtual void DebugImGui(); //< ImGuiデバッグ表示

    /// <summary>
    /// WVPデータの更新
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション行列</param>
    virtual void UpdateWVPData(const ViewProjection& viewProjection);

    // テクスチャ、モデルのセット
    void SetTextureByName(const std::string& name);
    void SetModelByName(const std::string& modelName);

public:
    ModelMaterial material_;    //< モデルのマテリアル
    ObjectColor objColor_;      //< オブジェクトの色
    WorldTransform transform_;  //< ワールド変換
protected:

    //*----------------------  protected variant ----------------------*//
    
    // WVP
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_;

    // シャドウマップ、モデル
    ShadowMap* shadowMap_ = nullptr;
    Model* model_         = nullptr;

    // 描画フラグ
    bool isShadow_ = true;
    bool isDraw_   = true;

    // 更新フラグ
    bool isAutoUpdate_ = true;

    uint32_t textureIndex_;
    BlendMode blendMode = BlendMode::None;

    const std::string textureFirePath_ = "Resources/Texture/ModelTexture/";

public:
    //*----------------------  Getter Methods ----------------------*//
    Model* GetModelPtr() { return model_; }
    int32_t GetTextureIndex() const { return textureIndex_; }
    const bool& GetIsAutoUpdate() const { return isAutoUpdate_; }
    //*----------------------  Setter Methods ----------------------*//
    void SetIsDraw(const bool& is) { isDraw_ = is; }
    void SetIsAutoUpdate(const bool& is) { isAutoUpdate_ = is; }
    void SetModel(Model* model) { model_ = model; }
    void SetBlendMode(const BlendMode& mode) { blendMode = mode; }
    void SetwvpDate(const Matrix4x4& date) { wvpDate_->WVP = date; }
    void SetWorldMatrixDate(const Matrix4x4& date) { wvpDate_->World = date; }
    void SetIsShadow(const bool& is) { isShadow_ = is; }
};