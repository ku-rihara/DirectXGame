#pragma once

// DirectX
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"

// Transform
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include "2d/Sprite.h"
#include "audio/Audio.h"
#include "input/Input.h"

// etc
#include "utility/Debug/DebugCamera.h"
#include <memory>

class BaseScene {

protected:
    DirectXCommon* dxCommon_        = nullptr;
    Input* input_                   = nullptr;
    Audio* audio_                   = nullptr;
    TextureManager* textureManager_ = nullptr;

    ViewProjection viewProjection_;

    std::unique_ptr<DebugCamera> debugCamera_;
    bool isDebugCameraActive_;

public:
    BaseScene()          = default;
    virtual ~BaseScene() = default;

    virtual void Init();
    virtual void Update()     = 0;
    virtual void SkyBoxDraw() = 0;

    /// <summary>
    /// デバッグ表示
    /// </summary>
    virtual void Debug();
    virtual void ViewProjectionUpdate();
    virtual void ViewProssess() = 0;

    ///========================================================
    /// getter method
    ///========================================================
    const ViewProjection& GetViewProjection() const { return viewProjection_; }

    ///========================================================
    /// setter method
    ///========================================================
};