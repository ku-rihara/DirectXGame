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

namespace KetaEngine {

class SpriteEaseAnimationPlayer;

/// <summary>
/// スプライトクラス
/// </summary>
class Sprite {
public:
    /// <summary>
    /// UV座標に対する拡縮・回転・平行移動を表す構造体
    /// </summary>
    struct UVTransform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };

    /// <summary>
    /// スプライトの拡縮・回転・座標を表す構造体
    /// </summary>
    struct Transform {
        Vector2 scale;
        Vector3 rotate;
        Vector2 pos;
    };

    /// <summary>
    /// GlobalParameterで編集されるパラメータ構造体
    /// </summary>
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

    /// <summary>
    /// スプライトの頂点・インデックスリソース等を構築する
    /// </summary>
    /// <param name="textureName">テクスチャのファイル名</param>
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

    /// <summary>
    /// ImGui上でパラメータを調整する
    /// </summary>
    void AdjustParam();

    /// <summary>
    /// パラメータをGlobalParameterに登録する
    /// </summary>
    void RegisterParams();

    /// <summary>
    /// スプライトの描画
    /// </summary>
    void Draw();

    /// <summary>
    /// 表示割合を設定(0.0f~1.0f)
    /// </summary>
    /// <param name="rate">表示割合(0.0f~1.0f)</param>
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

    /// <summary>
    /// GlobalParameterからパラメータを取得する
    /// </summary>
    void GetParams();

    /// <summary>
    /// イージングアニメーションの色・レートをマテリアルへ反映する
    /// </summary>
    void ApplyAnimationToMaterial();

    /// <summary>
    /// イージングアニメーションによる座標オフセットを取得する
    /// </summary>
    /// <returns>アニメーションによる座標オフセット</returns>
    Vector2 GetAnimationPosition() const;

    /// <summary>
    /// イージングアニメーションによる回転オフセットを取得する
    /// </summary>
    /// <returns>アニメーションによる回転オフセット</returns>
    Vector3 GetAnimationRotation() const;

public:
    // Transform
    Transform transform_;

private:
    // UVTransform
    UVTransform uvTransform_;

    // テクスチャ左上位置、アンカーポイント
    Vector2 textureLeftTop_; 
    Vector2 anchorPoint_; 

    // flip
    bool isFlipX_ = false;
    bool isFlipY_ = false;

    // パラメータ編集関連
    GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;
    const std::string filePath_   = "Resources/Texture/";
    const std::string folderPath_ = "SpriteParam";
    Parameter parameter_;

    // レイヤー、描画フラグ、初期パラメータ適用フラグ
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

    /// <summary>座標を取得する</summary>
    /// <returns>ワールド座標</returns>
    const Vector2& GetPosition() const { return transform_.pos; }

    /// <summary>描画レイヤー番号を取得する</summary>
    /// <returns>レイヤー番号</returns>
    int32_t GetLayerNum() const { return layerNum_; }

    /// <summary>編集用パラメータを取得する</summary>
    /// <returns>Parameter構造体</returns>
    const Parameter& GetValue() const { return parameter_; }

    /// <summary>描画フラグを取得する</summary>
    /// <returns>描画するか</returns>
    bool GetIsDraw() const { return isDraw_; };

    /// <summary>スプライトイージングアニメーションプレイヤーを取得する</summary>
    /// <returns>SpriteEaseAnimationPlayerのポインタ</returns>
    SpriteEaseAnimationPlayer* GetSpriteEaseAnimationPlayer() { return spriteEaseAnimationPlayer_.get(); }

    ///=========================================================================================
    ///  setter
    ///=========================================================================================

    /// <summary>アンカーポイントを設定する</summary>
    /// <param name="anchorPoint">アンカーポイント(0.0f~1.0f)</param>
    void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }

    /// <summary>テクスチャの切り抜き開始位置を設定する</summary>
    /// <param name="leftTop">切り抜き開始位置</param>
    void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }

    /// <summary>左右反転フラグを設定する</summary>
    /// <param name="isFlip">反転させるか</param>
    void SetIsFlipX(bool isFlip) { isFlipX_ = isFlip; }

    /// <summary>上下反転フラグを設定する</summary>
    /// <param name="isFlip">反転させるか</param>
    void SetIsFlipY(bool isFlip) { isFlipY_ = isFlip; }

    /// <summary>色を設定する</summary>
    /// <param name="color">RGB色</param>
    void SetColor(const Vector3& color);

    /// <summary>不透明度を設定する</summary>
    /// <param name="alpha">不透明度(0.0f~1.0f)</param>
    void SetAlpha(float alpha);

    /// <summary>描画レイヤー番号を設定する</summary>
    /// <param name="layerNum">レイヤー番号</param>
    void SetLayerNum(int32_t layerNum) { layerNum_ = layerNum; }

    /// <summary>描画フラグを設定する</summary>
    /// <param name="isDraw">描画するか</param>
    void SetIsDraw(bool isDraw) { isDraw_ = isDraw; }

    /// <summary>イージングアニメーションの再生速度倍率を設定する</summary>
    /// <param name="rate">再生速度倍率</param>
    void SetAnimationSpeedRate(float rate) { animationSpeedRate_ = rate; }

    /// <summary>UVTransformをまとめて設定する</summary>
    /// <param name="uvTransform">UVTransform構造体</param>
    void SetUVTransform(const UVTransform& uvTransform) { uvTransform_ = uvTransform; }

    /// <summary>UVスケールを設定する</summary>
    /// <param name="scale">UVスケール</param>
    void SetUVScale(const Vector2& scale) { uvTransform_.scale = scale; }

    /// <summary>UV回転を設定する</summary>
    /// <param name="rotate">UV回転</param>
    void SetUVRotate(const Vector3& rotate) { uvTransform_.rotate = rotate; }

    /// <summary>UV位置を設定する</summary>
    /// <param name="pos">UV位置</param>
    void SetUVPosition(const Vector2& pos) { uvTransform_.pos = pos; }
};

}; // KetaEngine
