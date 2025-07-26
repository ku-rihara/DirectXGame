#pragma once
#include "Material/ModelMaterial.h"
#include "Pipeline/Object3DPiprline.h"
#include "Model.h"

class BaseObject3d {
public:
    ModelMaterial material_;

protected:
    ///========================================================================================
    ///  protected variant
    ///========================================================================================

    Model* model_       = nullptr;
    BlendMode blendMode = BlendMode::None;

public:
    ///========================================================================================
    ///  public method
    ///========================================================================================

    BaseObject3d() = default;
    virtual ~BaseObject3d() = default;
    
    virtual void DebugImgui();
    virtual void CreateMaterialResource();

    ///========================================================================================
    ///  getter method
    ///========================================================================================
    Model* GetModel() { return model_; }

    ///========================================================================================
    ///  setter method
    ///========================================================================================

    void SetModel(Model* model) { this->model_ = model; }
    void SetBlendMode(BlendMode mode) { blendMode = mode; }
    void SetModel(const std::string& modelName);
};