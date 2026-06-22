#pragma once
// dx
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
// base
#include "Base/Material/SpriteMaterial.h"
// struct
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "GPUData/ModelData.h"
#include "GPUData/TransformationMatrix.h"
// std
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// スプライトクラス
/// </summary>
namespace KetaEngine {

class SpriteEaseAnimationPlayer;

class Sprite {
public:
    // UVTransformの構造体
    struct UVTransform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };

    // Transformの構造体
    struct Transform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };

    // Para
    struct Parameter {
        Vector2 position_         = Vector2::ZeroVector();
        Vector2 scale_            = Vector2::OneVector();
        Vector2 uvScale_          = Vector2::OneVector();
        Vector4 color_            = Vector4::kWHITE();
        Vector2 startAnchorPoint_ = Vector2::ZeroVector();
        int32_t startLayerNum_;
    };

public:
    Sprite() = default;
    ~Sprite();

    /// <summary>
    /// スプライトの作成
    /// </summary>
    /// <param name="textureName">テクスチャのファイル名</param>
    /// <param name="isAbleEdit">パラメータ編集を可能にするかのフラグ</param>
    /// <param name="index">スプライトを識別するための名前</param>
    /// <returns>作成されたSpriteのポインタ</returns>
    static Sprite* Create(const std::string& textureName, bool isAbleEdit = true, const std::string& name = "");
    void CreateSprite(const std::string& textureName);

    ///=========================================================================================
    ///  スプライトイージングアニメーション
    ///=========================================================================================

    /// <summary>
    /// スプライトイージングアニメーション再生
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    /// <param name="categoryName">カテゴリー名</param>
    void PlaySpriteEaseAnimation(const std::string& animationName, const std::string& categoryName = "Common");

    /// <summary>
    /// スプライトイージングアニメーション停止
    /// </summary>
    void StopSpriteEaseAnimation();

    /// <summary>
    /// スプライトイージングアニメーション更新
    /// </summary>
    void UpdateSpriteEaseAnimation();

    // パラメータ編集
    void AdjustParam();
    void RegisterParams();

    // 描画
    void Draw();

    /// <summary>
    /// 表示割合を設定(0.0f~1.0f)
    /// <summary>
    void SetDisplayRate(float rate);

private:
    /// <summary>
    /// パラメータ編集のグループに登録する
    /// </summary>
    void ParamEditorSet(const std::string& textureName, bool isAbleEditor, const std::string& name = "");

    /// <summary>
    /// Transformのパラメータを適用する
    /// </summary>
    void ApplyParameterToTransform();

    // パラメータ関連
    void GetParams();
    void ApplyAnimationToMaterial();
    Vector2 GetAnimationPosition() const;
    Vector3 GetAnimationRotation() const;

public:
    // Transform
    Transform transform_;

private:
    // UVTransform
    UVTransform uvTransform_; //< UVトランスフォーム
    Vector2 textureLeftTop_; //< テクスチャ左上
    Vector2 anchorPoint_; //< アンカーポイント

    // flip
    bool isFlipX_ = false;
    bool isFlipY_ = false;

    // パラメータ編集関連
    GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;
    const std::string filePath_   = "Resources/Texture/";
    const std::string folderPath_ = "SpriteParam";
    Parameter parameter_;

    // レイヤー。描画フラグ、初期パラメータ適用フラグ
    int32_t layerNum_;
    bool isDraw_           = true;
    bool isApplyInitParam_ = false;

    // 共有パラメータ管理
    bool isRepresentative_ = false;

    // 表示割合(0.0f~1.0f)
    float disPlayRate_ = 1.0f;

    // スプライトイージングアニメーション
    std::unique_ptr<SpriteEaseAnimationPlayer> spriteEaseAnimationPlayer_;
    float animationSpeedRate_ = 1.0f;

    // テクスチャ自体のサイズ
    Vector2 textureSize_;

    // バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;

    // テクスチャSRVのGPUハンドル、Index
    D3D12_GPU_DESCRIPTOR_HANDLE texture_;
    uint32_t textureIndex_;

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
    int32_t GetLayerNum() const { return layerNum_; }
    const Parameter& GetValue() const { return parameter_; }
    bool GetIsDraw() const { return isDraw_; };
    SpriteEaseAnimationPlayer* GetSpriteEaseAnimationPlayer() { return spriteEaseAnimationPlayer_.get(); }

    ///=========================================================================================
    ///  setter
    ///=========================================================================================

    void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
    void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }

    void SetIsFlipX(bool isFlip) { isFlipX_ = isFlip; }
    void SetIsFlipY(bool isFlip) { isFlipY_ = isFlip; }

    void SetColor(const Vector3& color);
    void SetAlpha(float alpha);

    void SetLayerNum(int32_t layerNum) { layerNum_ = layerNum; }
    void SetIsDraw(bool isDraw) { isDraw_ = isDraw; }
    void SetAnimationSpeedRate(float rate) { animationSpeedRate_ = rate; }

    // UVTransform
    void SetUVTransform(const UVTransform& uvTransform) { uvTransform_ = uvTransform; }
    void SetUVScale(const Vector2& scale) { uvTransform_.scale = scale; }
    void SetUVRotate(const Vector3& rotate) { uvTransform_.rotate = rotate; }
    void SetUVPosition(const Vector2& pos) { uvTransform_.pos = pos; }
};

}; // KetaEngine
