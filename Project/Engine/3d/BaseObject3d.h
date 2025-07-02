#pragma once
#include "base/Material.h"
#include "base/Object3DCommon.h"
#include "Model.h"

class BaseObject3d {
public:
    Material material_;

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

    // デバッグ表示
    virtual void DebugImgui();

    // マテリアルリソース作成
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