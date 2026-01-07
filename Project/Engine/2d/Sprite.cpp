#include "Sprite.h"

using namespace KetaEngine;
#include "base/TextureManager.h"
#include "base/WinApp.h"
#include "Dx/DirectXCommon.h"
#include "SpriteRegistry.h"
#include <imgui.h>

Sprite::~Sprite() {
    if (SpriteRegistry::GetInstance()) {
        SpriteRegistry::GetInstance()->UnregisterObject(this);
    }
}

Sprite* Sprite::Create(const std::string& textureName, bool isAbleEdit) {
    // 新しいSpriteインスタンスを作成
    std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();

    sprite->ParamEditorSet(textureName, isAbleEdit);
    sprite->CreateSprite(sprite->filePath_ + textureName);

    SpriteRegistry::GetInstance()->RegisterObject(sprite.get());
    return sprite.release();
}

void Sprite::ParamEditorSet(const std::string& textureName, bool isAbleEditor) {
    if (!isAbleEditor) {
        return;
    }
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // フォルダパスを除去してファイル名のみを取得
    std::string fileName = textureName;
    size_t lastSlash     = textureName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        fileName = textureName.substr(lastSlash + 1);
    }

    std::string uniqueGroupName = fileName;
    int32_t index               = 0;

    // グループが既に存在する場合、インデックスを追加
    while (SpriteRegistry::GetInstance()->HasGroupName(uniqueGroupName)) {
        index++;
        uniqueGroupName = fileName + std::to_string(index);
    }

    groupName_ = uniqueGroupName;

    // グループ作成
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void Sprite::CreateSprite(const std::string& textureName) {
    DirectXCommon* directXCommon = DirectXCommon::GetInstance();

    // テクスチャ読み込み
    textureIndex_ = TextureManager::GetInstance()->LoadTexture(textureName);
    texture_      = TextureManager::GetInstance()->GetTextureHandle(textureIndex_);

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
    // インデックスはUint32_tとする
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

    // UVTransformは単位行列を書き込んでおく
    material_.GetMaterialData()->uvTransform = MakeIdentity4x4();
    uvTransform_.scale                       = {1.0f, 1.0f};

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
    transform_.pos                     = parameter_.position_;
    transform_.scale                   = Vector2::OneVector();
    uvTransform_.scale                 = parameter_.uvScale_;
    material_.GetMaterialData()->color = parameter_.color_;

    anchorPoint_ = parameter_.startAnchorPoint_;
    layerNum_    = parameter_.startLayerNum_;

    // テクスチャ座標取得
    const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex_);
    textureSize_.x                       = static_cast<float>(metadata.width);
    textureSize_.y                       = static_cast<float>(metadata.height);
}

void Sprite::Draw() {
    DirectXCommon* directXCommon = DirectXCommon::GetInstance();

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

    if (isFlipY_) { /// 上下反転
        top    = -top;
        bottom = -bottom;
    }

    ///==========================================================================================
    //  TextureClip
    ///==========================================================================================

    float texLeft   = textureLeftTop_.x / textureSize_.x;
    float texRight  = (textureLeftTop_.x + textureSize_.x) / textureSize_.x;
    float texTop    = textureLeftTop_.y / textureSize_.y;
    float texBottom = (textureLeftTop_.y + textureSize_.y) / textureSize_.y;

    // 頂点リソースにデータ書込む
    vertexData_[0].texcoord = {texLeft, texBottom};
    vertexData_[1].texcoord = {texLeft, texTop};
    vertexData_[2].texcoord = {texRight, texBottom};
    vertexData_[3].texcoord = {texRight, texTop};

    ///==========================================================================================
    //  Transform
    ///==========================================================================================

    Vector3 size      = Vector3(textureSize_.x, textureSize_.y, 0.0f) * Vector3(transform_.scale.x * parameter_.scale_.x, transform_.scale.y * parameter_.scale_.y, 0.0f);
    Vector3 translate = Vector3(transform_.pos.x, transform_.pos.y, 0.0f);

    // 行列変換
    Matrix4x4 worldMatrixSprite               = MakeAffineMatrix(size, transform_.rotate, translate);
    Matrix4x4 projectionMatrixSprite          = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
    Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * projectionMatrixSprite;

    ///==========================================================================================
    //  UVTransform
    ///==========================================================================================

    material_.GetMaterialData()->uvTransform = MakeAffineMatrix(
        Vector3{uvTransform_.scale.x, uvTransform_.scale.y, 1.0f}, /// scale
        uvTransform_.rotate, /// rotate
        Vector3{uvTransform_.pos.x, uvTransform_.pos.y, 1.0f}); /// translate

    wvpData_->WVP = worldViewProjectionMatrixSprite;

    directXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
    directXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_); // IBVを設定

    // TransFormationMatrixCBufferの場所を設定
    material_.SetCommandList(directXCommon->GetCommandList());
    directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_);
    // 描画(DrawCall/ドローコール)
    directXCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

///=========================================================
/// バインド
///==========================================================
void Sprite::RegisterParams() {
    globalParameter_->Regist(groupName_, "layerNum", &parameter_.startLayerNum_);
    globalParameter_->Regist(groupName_, "startPosition", &parameter_.position_);
    globalParameter_->Regist(groupName_, "startScale", &parameter_.scale_);
    globalParameter_->Regist(groupName_, "startColor", &parameter_.color_);
    globalParameter_->Regist(groupName_, "startUVScale", &parameter_.uvScale_);
    globalParameter_->Regist(groupName_, "startAnchorPoint", &parameter_.startAnchorPoint_);
}

///=========================================================
/// パラメータ調整
///==========================================================
void Sprite::AdjustParam() {
#ifdef _DEBUG
    if (groupName_.empty()) {
        return;
    }
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("LayerNum");
        ImGui::InputInt("LayerNum", &parameter_.startLayerNum_);

        ImGui::SeparatorText("InitParam");
        ImGui::DragFloat2("StartPos", &parameter_.position_.x, 0.1f);
        ImGui::DragFloat2("baseScale", &parameter_.scale_.x, 0.1f);
        ImGui::DragFloat2("StartAnchorPoint", &parameter_.startAnchorPoint_.x, 0.01f);
        ImGui::ColorEdit4("StartColor", &parameter_.color_.x);
        ImGui::SeparatorText("UVParam");
        ImGui::DragFloat2("UVScale", &parameter_.uvScale_.x, 0.01f);

        ImGui::Checkbox("isAdaptStartParam", &isAdaptStartParam_);
        if (isAdaptStartParam_) {
            // 適応
            transform_.pos                     = parameter_.position_;
            transform_.scale                   = parameter_.scale_;
            uvTransform_.scale                 = parameter_.uvScale_;
            material_.GetMaterialData()->color = parameter_.color_;
            anchorPoint_                       = parameter_.startAnchorPoint_;
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
        globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void Sprite::SetColor(const Vector3& color) {
    material_.GetMaterialData()->color.x = color.x;
    material_.GetMaterialData()->color.y = color.y;
    material_.GetMaterialData()->color.z = color.z;
}

void Sprite::SetAlpha(float alpha) {
    material_.GetMaterialData()->color.w = alpha;
}

void Sprite::SetTextureDisplaySize(const Vector2& size) {
  
    Vector2 clampedSize;
    clampedSize.x = std::clamp(size.x, 0.0f, textureSize_.x);
    clampedSize.y = std::clamp(size.y, 0.0f, textureSize_.y);

    // テクスチャ座標の右端と下端を計算
    textureSize_ = clampedSize;
}