#include "Sprite.h"
#include "Pipeline/SpritePipeline.h"
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"
#include"base/WinApp.h"
#include <imgui.h>

namespace {
DirectXCommon* directXCommon = DirectXCommon::GetInstance();
// Model* model=Model::GetInstance();
}

Sprite* Sprite::Create(const uint32_t& textureHandle, const Vector2& position, const Vector4& color) {
    // 新しいModelインスタンスを作成
    Sprite* sprite = new Sprite();
    sprite->CreateSprite(textureHandle, position, color);
    return sprite; // 成功した場合は新しいモデルを返す
}

void Sprite::CreateSprite(const uint32_t& textureHandle, const Vector2& position, const Vector4& color) {
    // テクスチャ
    texture_      = TextureManager::GetInstance()->GetTextureHandle(textureHandle);
    textureIndex_ = textureHandle;

    // Sprite用の頂点リソースを作る
    vertexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * 4);
    // 頂点バッファビューを作成する
    vertexBufferView_ = {};
    // リソースの先頭のアドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    // 使用するリソースのサイズは頂点6つ分ののサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
    // 頂点当たりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    vertexData_ = nullptr;
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    // 頂点データ
    vertexData_[0].position = {0.0f, 360.0f, 0.0f, 1.0f}; // 左下
    vertexData_[0].texcoord = {0.0f, 1.0f};
    vertexData_[1].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 左上
    vertexData_[1].texcoord = {0.0f, 0.0f};
    vertexData_[2].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下
    vertexData_[2].texcoord = {1.0f, 1.0f};
    vertexData_[3].position = {640.0f, 0.0f, 0.0f, 1.0f}; // 右上
    vertexData_[3].texcoord = {1.0f, 0.0f};

    // 頂点インデックス
    indexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(uint32_t) * 6);
    // リソースの先頭アドレスから使う
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    // 使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
    // インデックスはuint32_tとする
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
    // インデックスリソースにデータを書き込む
    uint32_t* indexDataSprite = nullptr;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
    indexDataSprite[0] = 0;
    indexDataSprite[1] = 1;
    indexDataSprite[2] = 2;
    indexDataSprite[3] = 1;
    indexDataSprite[4] = 3;
    indexDataSprite[5] = 2;

    ///==========================================================================================
    //  マテリアル
    ///==========================================================================================
    material_.CreateMaterialResource(directXCommon);
    // Lightingを無効
    material_.materialData_->color = color;
    // UVTransformは単位行列を書き込んでおく
    material_.materialData_->uvTransform = MakeIdentity4x4();
    uvTransform_.scale                   = {1, 1};
    ///==========================================================================================
    //  WVP
    ///==========================================================================================
    wvpResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix2D));
    // データを書き込む
    wvpData_ = nullptr;
    // 書き込むためのアドレスを取得
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
    // 単位行列を書き込んでおく
    wvpData_->WVP = MakeIdentity4x4();
    ///==========================================================================================
    //  変数初期化
    ///==========================================================================================
    transform_.translate = {position.x, position.y, 1.0f};
    transform_.scale     = {1, 1, 1};
    // テクスチャ座標取得
    const DirectX::TexMetadata& metadata =
        TextureManager::GetInstance()->GetMetaData(textureHandle);
    textureAdjustSize_.x = static_cast<float>(metadata.width);
    textureAdjustSize_.y = static_cast<float>(metadata.height);
    /// テクスチャサイズに合わせる

    textureSize_ = textureAdjustSize_;
}

void Sprite::DebugImGui() {
#ifdef _DEBUG
    ImGui::ColorEdit4(" Color", (float*)&material_.materialData_->color);
    if (ImGui::TreeNode("Transform")) {
        ImGui::DragFloat3(" pos", &transform_.translate.x);
        ImGui::DragFloat2("size", &textureSize_.x);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("UV")) {
        ImGui::DragFloat2(" uvScale", (float*)&uvTransform_.scale.x);
        ImGui::DragFloat3(" uvRotate", (float*)&uvTransform_.rotate.x);
        ImGui::DragFloat2(" uvTransform", (float*)&uvTransform_.pos.x, 0.1f);
        ImGui::DragFloat2(" anchorPoint", (float*)&anchorPoint_.x, 0.1f);

        ImGui::TreePop();
    }
#endif
}


void Sprite::Draw() {

    ///==========================================================================================
    //  anchorPoint
    ///==========================================================================================

    float left   = 0.0f - anchorPoint_.x;
    float right  = 1.0f - anchorPoint_.x;
    float top    = 0.0f - anchorPoint_.y;
    float bottom = 1.0f - anchorPoint_.y;

    vertexData_[0].position = {left, bottom, 0.0f, 1.0f};
    vertexData_[1].position = {left, top, 0.0f, 1.0f};
    vertexData_[2].position = {right, bottom, 0.0f, 1.0f};
    vertexData_[3].position = {right, top, 0.0f, 1.0f};

    /// フリップ処理
    if (isFlipX_) { /// 左右反転
        left  = -left;
        right = -right;
    }

    if (isFlipX_) { /// 上下反転
        top    = -top;
        bottom = -bottom;
    }

    ///==========================================================================================
    //  TextureClip
    ///==========================================================================================

    float texLeft   = textureLeftTop_.x / textureAdjustSize_.x;
    float texRight  = (textureLeftTop_.x + textureSize_.x) / textureAdjustSize_.x;
    float texTop    = textureLeftTop_.y / textureAdjustSize_.y;
    float texBottom = (textureLeftTop_.y + textureSize_.y) / textureAdjustSize_.y;

    // 頂点リソースにデータ書き込む
    vertexData_[0].texcoord = {texLeft, texBottom};
    vertexData_[1].texcoord = {texLeft, texTop};
    vertexData_[2].texcoord = {texRight, texBottom};
    vertexData_[3].texcoord = {texRight, texTop};

    ///==========================================================================================
    //  Transform
    ///==========================================================================================
    Matrix4x4 worldMatrixSprite               = MakeAffineMatrix(Vector3(textureSize_.x, textureSize_.y, 0) * (transform_.scale), transform_.rotate, transform_.translate);
    Matrix4x4 projectionMatrixSprite          = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
    Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * projectionMatrixSprite;

    ///==========================================================================================
    //  UVTransform
    ///==========================================================================================

    material_.materialData_->uvTransform = MakeAffineMatrix(
        Vector3{uvTransform_.scale.x, uvTransform_.scale.y, 1.0f}, /// scale
        uvTransform_.rotate, /// rotate
        Vector3{uvTransform_.pos.x, uvTransform_.pos.y, 1.0f}); /// translate

    wvpData_->WVP = worldViewProjectionMatrixSprite;

    directXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
    directXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_); // IBVを設定

    // TransformationmatrixCBufferの場所を設定
    material_.SetCommandList(directXCommon->GetCommandList());
    directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_);
    // 描画(DrawCall/ドローコール)
    directXCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::SetPosition(const Vector2& pos) {

    transform_.translate.x = pos.x;
    transform_.translate.y = pos.y;
}

void Sprite::SetScale(const Vector2& scale) {

    transform_.scale.x = scale.x;
    transform_.scale.y = scale.y;
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {
    SpritePipeline::GetInstance()->PreDraw(commandList);
}

void Sprite::SetColor(const Vector3& color) {
    material_.materialData_->color.x = color.x;
    material_.materialData_->color.y = color.y;
    material_.materialData_->color.z = color.z;
}

void Sprite::SetAlpha(const float& alpha) {
    material_.materialData_->color.w = alpha;
}