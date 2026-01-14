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
namespace KetaEngine {

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
    WorldTransform transform_; //< ワールド変換
protected:
    //*----------------------  protected variant ----------------------*//

    std::unique_ptr<ModelMaterial> material_; //< モデルのマテリアル

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
    Model* GetModel() const { return model_; }
    int32_t GetTextureIndex() const { return textureIndex_; }
    bool GetIsAutoUpdate() const { return isAutoUpdate_; }
    ModelMaterial* GtModelMaterial() const { return material_.get(); }
    //*----------------------  Setter Methods ----------------------*//
    void SetIsDraw(bool is) { isDraw_ = is; }
    void SetIsAutoUpdate(bool is) { isAutoUpdate_ = is; }
    void SetModel(Model* model) { model_ = model; }
    void SetBlendMode(const BlendMode& mode) { blendMode = mode; }
    void SetwvpDate(const Matrix4x4& date) { wvpDate_->WVP = date; }
    void SetWorldMatrixDate(const Matrix4x4& date) { wvpDate_->World = date; }
    void SetIsShadow(bool is) { isShadow_ = is; }
};

}; // KetaEngine
