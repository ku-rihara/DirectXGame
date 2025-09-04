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

class BaseObject3d {
public:
    ///========================================================================================
    ///  public method
    ///========================================================================================

    BaseObject3d()          = default;
    virtual ~BaseObject3d() = default;

    virtual void DebugImgui();
    virtual void CreateMaterialResource();
    virtual void CreateShadowMap();
    virtual void CreateWVPResource();
    virtual void UpdateWVPData(const ViewProjection& viewProjection);

public:
    ModelMaterial material_;
    ObjectColor objColor_;
    WorldTransform transform_;

protected:
    ///========================================================================================
    ///  protected variant
    ///========================================================================================

    // wvpリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_;
    ShadowMap* shadowMap_;
    bool isShadow_ = true;
    bool isDraw_   = true;
    uint32_t textureIndex_;

    Model* model_       = nullptr;
    BlendMode blendMode = BlendMode::None;

    const std::string textureFirePath_ = "Resources/Texture/ModelTexture/";

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
    void SetTexture(const std::string& name);
    void SetModel(Model* model) { model_ = model; }
    void SetBlendMode(BlendMode mode) { blendMode = mode; }
    void SetModel(const std::string& modelName);
    void SetwvpDate(Matrix4x4 date) { wvpDate_->WVP = date; }
    void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
    void SetIsShadow(const bool& is) { isShadow_ = is; }
};