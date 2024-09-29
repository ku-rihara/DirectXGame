#include "Sprite.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include <imgui.h>

namespace {
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	//Model* model=Model::GetInstance();
}
// static メンバ変数の定義
D3D12_VERTEX_BUFFER_VIEW Sprite::vertexBufferViewSprite_;
D3D12_INDEX_BUFFER_VIEW Sprite::indexBufferViewSprite_;

void Sprite::CreateSprite(uint32_t textureHandle, Vector2 position, Vector4 color) {
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
	materialResourceSprite_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	materialDateSprite_ = nullptr;
	//書き込むためのアドレスを取得
	materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&materialDateSprite_));
	//Lightingを無効
	materialDateSprite_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialDateSprite_->enableLighting = false;
	//UVTransformは単位行列を書き込んでおく
	materialDateSprite_->uvTransform = MakeIdentity4x4();
		//行列----------------------------------------------------------------------------------------------------------
	wvpResourceSprite_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	wvpDataSprite_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite_));
	//単位行列を書き込んでおく
	wvpDataSprite_->World = MakeIdentity4x4();
	wvpDataSprite_->WVP = MakeIdentity4x4();
//変数初期化-----------------------------------------------------------
	
}

#ifdef _DEBUG
void Sprite::DebugImGui() {
	/*ImGui::Begin("Lighting");*/
	ImGui::ColorEdit4(" Color", (float*)&materialDateSprite_->color);
}
#endif

void Sprite::DrawSprite(D3D12_GPU_DESCRIPTOR_HANDLE texture) {

	//TransformationmatrixCBufferの場所を設定
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceSprite_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture);
	//描画(DrawCall/ドローコール)
	directXCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList){
	////Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	commandList->IASetIndexBuffer(&indexBufferViewSprite_);//IBVを設定
}

void Sprite::SetPosition(const Vector2& pos) {

}

//void Sprite::ReleaseSprite() {
//	vertexResourceSprite_->Release();
//	indexResourceSprite_->Release();
//	wvpResourceSprite_->Release();
//	materialResourceSprite_->Release();
//}