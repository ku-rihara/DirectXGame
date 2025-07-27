#pragma once

#include "Animation/SkinCluster.h"
#include "BaseObject3d.h"
#include "ObjectColor.h"
#include "ShadowMap/ShadowMap.h"

#include "3d/WorldTransform.h"
#include "struct/TransformationMatrix.h"
#include <memory>

class Object3d : public BaseObject3d {
public:
    Object3d()  = default;
    ~Object3d() = default;

    ///============================================================
    /// public method
    ///============================================================

    /// モデル作成
    static Object3d* CreateModel(const std::string& instanceName);

    /// 初期化、更新、描画
    void Init();
    void ColorUpdate();
    void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
    void Draw(const Vector3& position, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);

    // アニメーション描画
    void DrawAnimation(const WorldTransform& worldTransform, const ViewProjection& viewProjection, SkinCluster bufferView, std::optional<uint32_t> textureHandle = std::nullopt);

    void DebugImgui() override;
    void CreateWVPResource();
    void CreateShadowMap();
    void CreateMaterialResource() override;

private:

    void ShadowDraw();

public:
    ObjectColor objColor_;

private:
    ///============================================================
    /// private variant
    ///============================================================

    // wvpリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_;
    ShadowMap* shadowMap_;
    bool isShadow_ = false;

    ///============================================================
    /// setter method
    ///============================================================
    void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
    void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
    void SetIsShadow(const bool& is) { isShadow_ = is; }
};