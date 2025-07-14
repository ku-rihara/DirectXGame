#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
// struct
#include "base/SpriteMaterial.h"
#include "struct/ModelData.h"
#include "struct/TransformationMatrix.h"

class Sprite {
public:

    Sprite() = default;
    ~Sprite() = default;

    static Sprite* Create(const uint32_t& textureHandle, const Vector2& position, const Vector4& color);
    /// スプライトの作成
    void CreateSprite(const uint32_t& textureHandle, const Vector2& position, const Vector4& color);
    // 描画前
    static void PreDraw(ID3D12GraphicsCommandList* commandList);
    // 描画
    void Draw();
    // ポジションセット
    void SetPosition(const Vector2& pos);
    // スプライトセット
    void SetScale(const Vector2& scale);
    void DebugImGui();

public:
    struct UVTransform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };
    struct Transform {
        Vector3 scale;
        Vector3 rotate;
        Vector3 translate;
    };

    /// Transform
    Transform transform_; /// トランスフォーム
    UVTransform uvTransform_; /// UVトランスフォーム
    Vector2 textureLeftTop_; /// テクスチャ左上
    Vector2 textureSize_; /// テクスチャサイズ

    /// anchorPoint
    Vector2 anchorPoint_;

    /// flip
    bool isFlipX_ = false; /// FlipX
    bool isFlipY_ = false; /// FlipY

private:
    Vector2 textureAdjustSize_; /// テクスチャサイズ

    // テクスチャ
    D3D12_GPU_DESCRIPTOR_HANDLE texture_;
    uint32_t textureIndex_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;
    // リソース******************************************************************
    // 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    // wvpリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    // indexリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    // データ****************************************************************************
    TransformationMatrix2D* wvpData_;
    VertexData* vertexData_;
    SpriteMaterial material_;

public:
    ///=========================================================================================
    ///  getter
    ///=========================================================================================
    Vector2 GetPosition() const { return Vector2(transform_.translate.x, transform_.translate.y); }
    const Vector2& GetAnchorPoint() const { return anchorPoint_; }
    const Vector2& GetTextureSize() const { return textureSize_; }
    const Vector2& GetTextureJustSize() const { return textureAdjustSize_; }
    const Vector2& GetTextureLeftTop() const { return textureLeftTop_; }
    const bool& GetIsFlipX() const { return isFlipX_; }
    const bool& GetIsFlipY() const { return isFlipY_; }

    ///=========================================================================================
    ///  setter
    ///=========================================================================================
    void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
    void SetTextureSize(const Vector2& size) { textureSize_ = size; }
    void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }

    void SetIsFlipX(const bool& isFlip) { isFlipX_ = isFlip; }
    void SetIsFlipY(const bool& isFlip) { isFlipY_ = isFlip; }

    void SetColor(const Vector3& color);
    void SetAlpha(const float& alpha);

    void SetTransformationMatrixDataSprite(Matrix4x4 date) { wvpData_->WVP = date; }
    void SetWorldMatrixDataSprite(Matrix4x4 date) { wvpData_->World = date; }

    void SetUVTranslate(const Vector2& pos) { uvTransform_.pos = pos; }
    void SetUVScale(const Vector2& scale) { uvTransform_.scale = scale; }
};
