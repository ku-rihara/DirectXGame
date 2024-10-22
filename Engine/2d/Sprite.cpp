#include "Sprite.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include"SpriteCommon.h"
#include <imgui.h>

namespace {
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	//Model* model=Model::GetInstance();
}
// static メンバ変数の定義
D3D12_VERTEX_BUFFER_VIEW Sprite::vertexBufferView_;
D3D12_INDEX_BUFFER_VIEW Sprite::indexBufferView_;

Sprite* Sprite::Create(const uint32_t& textureHandle, const Vector2& position, const Vector4& color) {
	// 新しいModelインスタンスを作成
	Sprite* sprite = new Sprite();
	sprite->CreateSprite(textureHandle,position,color);
	return sprite;  // 成功した場合は新しいモデルを返す
}


void Sprite::CreateSprite(const uint32_t& textureHandle, const Vector2& position, const Vector4& color) {
	//テクスチャ
	texture_ = TextureManager::GetInstance()->GetTextureHandle(textureHandle);

	//スプライト**************************************************************************************************
	//Sprite用の頂点リソースを作る
	vertexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * 4);
	//頂点バッファビューを作成する
	vertexBufferView_ = {};
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分ののサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//頂点データ
	vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f }; // 左下
	vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };  // 左上
	vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f }; // 右下
	vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
	vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f }; // 右上
	vertexDataSprite[3].texcoord = { 1.0f, 0.0f };

	//頂点インデックス
	indexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(uint32_t) * 6);
	//リソースの先頭アドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	//マテリアル--------------------------------------------------------------------------------------
	material_.CreateMaterialResource(directXCommon);
	//Lightingを無効
	material_.materialData_->color = color;
	material_.materialData_->enableLighting = false;
	//UVTransformは単位行列を書き込んでおく
	material_.materialData_->uvTransform = MakeIdentity4x4();
		//行列----------------------------------------------------------------------------------------------------------
	wvpResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix2D));
	//データを書き込む
	wvpData_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	wvpData_->WVP = MakeIdentity4x4();
//変数初期化-----------------------------------------------------------
	transform_.translate = { position.x,position.y ,1.0f};
	transform_.scale = { 1,1,1 };
}

#ifdef _DEBUG
void Sprite::DebugImGui() {
	/*ImGui::Begin("Lighting");*/
	ImGui::ColorEdit4(" Color", (float*)&material_.materialData_->color);
	ImGui::DragFloat2(" uvScale", (float*)&uvTransform_.scale.x);
	ImGui::DragFloat3(" uvRotate", (float*)&uvTransform_.rotate.x);
	ImGui::DragFloat2(" uvTransform", (float*)&uvTransform_.pos.x);
}
#endif

void Sprite::Draw() {

	//スプライト
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite*projectionMatrixSprite;

	//UVTransform
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ uvTransform_.scale.x,uvTransform_.scale.y,1.0f });
	uvTransformMatrix = (uvTransformMatrix * MakeRotateZMatrix(uvTransform_.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * MakeTranslateMatrix(Vector3{ uvTransform_.pos.x,uvTransform_.pos.y,1.0f }));
	material_.materialData_->uvTransform = uvTransformMatrix;

	wvpData_->WVP=worldViewProjectionMatrixSprite;

	directXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定

	//TransformationmatrixCBufferの場所を設定
	material_.SetCommandList(directXCommon->GetCommandList());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_);
	//描画(DrawCall/ドローコール)
	directXCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}



void Sprite::SetUVTransform() {

	////UVTransform
	//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{uvTransform.scale.x,uvTransform.scale.y,1.0f});
	//uvTransformMatrix = (uvTransformMatrix * MakeRotateZMatrix(uvTransform.rotate.z));
	//uvTransformMatrix = (uvTransformMatrix * MakeTranslateMatrix(Vector3{ uvTransform.pos.x,uvTransform.pos.y,1.0f }));
	//material_.materialData_->uvTransform = uvTransformMatrix;
}
void Sprite::SetPosition(const Vector2& pos) {

	transform_.translate.x = pos.x;
	transform_.translate.y = pos.y;

}

void Sprite::SetScale(const Vector2& scale) {

	transform_.scale.x = scale.x;
	transform_.scale.y = scale.y;

}

void  Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {
	SpriteCommon::GetInstance()->PreDraw(commandList);
}
