#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
// base
#include "base/SpriteMaterial.h"
// struct
#include "struct/ModelData.h"
#include "struct/TransformationMatrix.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <string>

class Sprite {
public:
    struct UVTransform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };
    struct Transform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };

    struct Parameter {
        Vector2 position_         = Vector2::ZeroVector();
        Vector2 scale_            = Vector2::UnitVector();
        Vector2 uvScale_          = Vector2::UnitVector();
        Vector4 color_            = Vector4::kWHITE();
        Vector2 startAnchorPoint_ = Vector2::UnitVector();
        int32_t startLayerNum_;
    };

public:
    Sprite() = default;
    ~Sprite();

    // スプライトの作成
    static Sprite* Create(const std::string& textureName, const bool& isAbleEdit = true);
    void CreateSprite(const std::string& textureName);

    // editor
    void AdjustParam();
    void BindParams();

    // 描画
    void Draw();

private:
    void ParamEditorSet(const std::string& textureName, const bool& isAbleEditor);

public:
    // Transform
    Transform transform_; //< トランスフォーム
    UVTransform uvTransform_; //< UVトランスフォーム
    Vector2 textureLeftTop_; //< テクスチャ左上

    // flip
    bool isFlipX_ = false;
    bool isFlipY_ = false;

    Vector2 anchorPoint_;

private:
    // パラメータ編集
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string filePath_   = "Resources/Texture/";
    const std::string folderPath_ = "SpriteParam";
    Parameter parameter_;

    int32_t layerNum_;
    bool isDraw_ = true;
    bool isAdaptStartParam_=false;

    // テクスチャ
    Vector2 textureSize_; //< テクスチャ自体のサイズ
    D3D12_GPU_DESCRIPTOR_HANDLE texture_;
    uint32_t textureIndex_;

    // view
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;

    // Resource
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_; //< 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_; //< wvpリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_; //<indexリソース

    // データ
    TransformationMatrix2D* wvpData_;
    VertexData* vertexData_;
    SpriteMaterial material_;

public:
    ///=========================================================================================
    ///  getter
    ///=========================================================================================
    const Vector2& GetPosition() const { return transform_.pos; }
    const std::string& GetGroupName() const { return groupName_; }
    const Vector2& GetAnchorPoint() const { return anchorPoint_; }
    const Vector2& GetTextureSize() const { return textureSize_; }
    const Vector2& GetTextureLeftTop() const { return textureLeftTop_; }
    const int32_t& GetLayerNum() const { return layerNum_; }
    const Parameter& GetStartParameter() const { return parameter_; }
    const bool& GetIsFlipX() const { return isFlipX_; }
    const bool& GetIsFlipY() const { return isFlipY_; }
    const bool& GetIsDraw() const { return isDraw_; };

    ///=========================================================================================
    ///  setter
    ///=========================================================================================

    void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
    void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }

    void SetIsFlipX(const bool& isFlip) { isFlipX_ = isFlip; }
    void SetIsFlipY(const bool& isFlip) { isFlipY_ = isFlip; }

    void SetColor(const Vector3& color);
    void SetAlpha(const float& alpha);

    void SetLayerNum(const int32_t& layerNum) { layerNum_ = layerNum; }
    void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }
};
