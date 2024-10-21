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
D3D12_VERTEX_BUFFER_VIEW Sprite::vertexBufferViewSprite_;
D3D12_INDEX_BUFFER_VIEW Sprite::indexBufferViewSprite_;

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
	vertexResourceSprite_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * 4);
	//頂点バッファビューを作成する
	vertexBufferViewSprite_ = {};
	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分ののサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	//頂点当たりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
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
	indexResourceSprite_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(uint32_t) * 6);
	//リソースの先頭アドレスから使う
	indexBufferViewSprite_.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;
	//インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
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
	wvpResourceSprite_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix2D));
	//データを書き込む
	wvpDataSprite_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite_));
	//単位行列を書き込んでおく
	wvpDataSprite_->WVP = MakeIdentity4x4();
//変数初期化-----------------------------------------------------------
	transform_.translate = { position.x,position.y };
	transform_.scale = { 1,1 };
}

#ifdef _DEBUG
void Sprite::DebugImGui() {
	/*ImGui::Begin("Lighting");*/
	ImGui::ColorEdit4(" Color", (float*)&material_.materialData_->color);
}
#endif

void Sprite::Draw() {

	//スプライト
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(Vector3{ transform_.scale.x, transform_.scale.y,0 }, Vector3{ transform_.rotate.x, transform_.rotate.y,0 }, Vector3{ transform_.translate.x, transform_.translate.y,0 });
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite*projectionMatrixSprite;

	wvpDataSprite_->WVP=worldViewProjectionMatrixSprite;

	//TransformationmatrixCBufferの場所を設定
	material_.SetCommandList(directXCommon->GetCommandList());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceSprite_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_);
	//描画(DrawCall/ドローコール)
	directXCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}



void Sprite::SetUVTransform(const UVTransform& uvTransform) {

	//UVTransform
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{uvTransform.scale.x,uvTransform.scale.y,0.0f});
	uvTransformMatrix = (uvTransformMatrix * MakeRotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * MakeTranslateMatrix(Vector3{ uvTransform.pos.x,uvTransform.pos.y,0.0f }));
	material_.materialData_->uvTransform = uvTransformMatrix;
}
void Sprite::SetPosition(const Vector2& pos) {

	transform_.translate =pos;

}

void Sprite::SetScale(const Vector2& scale) {

	transform_.scale = scale;

}

void  Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {
	SpriteCommon::GetInstance()->PreDraw(commandList);
}
