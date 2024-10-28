#include "Mesh.h"
#include "base/DirectXCommon.h"
#include "base/TextureManager.h"
#include<imgui.h>
//struct
#include"struct/VertexData.h"
#include<format>

namespace {
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	
}

Mesh* Mesh::GetInstance() {
	static Mesh instance;
	return &instance;
}

void Mesh::CreateSphere() {
	//三角形***********************************************************************************************************************
		//VertexBufferViewを作成する	
	vertexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * shpereVertexNum_);
	//頂点バッファビューを作成する
	vertexBufferView_ = {};
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * shpereVertexNum_;
	//頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	VertexData* vertexDate = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));

	//頂点インデックス
	indexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(uint32_t) * shpereVertexNum_);

	//リソースの先頭アドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * shpereVertexNum_;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	float pi = 3.1415926535f;//π
	//経度分割1つ分の角度φ
	const float kLonEvery = pi * 2.0f / float(kSubdivision_);
	//緯度分割1つ分の角度φ
	const float kLatEvery = pi / float(kSubdivision_);
	vertexDate[0].normal = { 0.0f,0.0f,-1.0f };
	for (uint32_t latIndex = 0; latIndex < kSubdivision_; latIndex++) {	//緯度の方向に分割
		float lat = -pi / 2.0f + kLatEvery * latIndex;//θ
		float v = 1.0f - float(latIndex) / float(kSubdivision_);

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {//経度の方向に分割しながら
			float u = float(lonIndex) / float(kSubdivision_);
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;//φ
			//下
			vertexDate[start].position.x = cos(lat) * cos(lon);
			vertexDate[start].position.y = sin(lat);
			vertexDate[start].position.z = cos(lat) * sin(lon);
			vertexDate[start].position.w = 1.0f;
			vertexDate[start].texcoord = { u,v };
			vertexDate[start].normal.x = vertexDate[start].position.x;
			vertexDate[start].normal.y = vertexDate[start].position.y;
			vertexDate[start].normal.z = vertexDate[start].position.z;
			indexData[start] = start;

			//b
			vertexDate[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexDate[start + 1].position.y = sin(lat + kLatEvery);
			vertexDate[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexDate[start + 1].position.w = 1.0f;
			vertexDate[start + 1].texcoord = { u, v - (1.0f / float(kSubdivision_)) };
			vertexDate[start + 1].normal.x = vertexDate[start + 1].position.x;
			vertexDate[start + 1].normal.y = vertexDate[start + 1].position.y;
			vertexDate[start + 1].normal.z = vertexDate[start + 1].position.z;
			indexData[start + 1] = start + 1;
			//c
			vertexDate[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexDate[start + 2].position.y = sin(lat);
			vertexDate[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexDate[start + 2].position.w = 1.0f;
			vertexDate[start + 2].texcoord = { u + (1.0f / float(kSubdivision_)), v };
			vertexDate[start + 2].normal.x = vertexDate[start + 2].position.x;
			vertexDate[start + 2].normal.y = vertexDate[start + 2].position.y;
			vertexDate[start + 2].normal.z = vertexDate[start + 2].position.z;
			indexData[start + 2] = start + 2;
			//上//b
			vertexDate[start + 3].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexDate[start + 3].position.y = sin(lat + kLatEvery);
			vertexDate[start + 3].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexDate[start + 3].position.w = 1.0f;
			vertexDate[start + 3].texcoord = { u, v - (1.0f / float(kSubdivision_)) };
			vertexDate[start + 3].normal.x = vertexDate[start + 3].position.x;
			vertexDate[start + 3].normal.y = vertexDate[start + 3].position.y;
			vertexDate[start + 3].normal.z = vertexDate[start + 3].position.z;
			indexData[start + 3] = start + 3;
			//d
			vertexDate[start + 4].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexDate[start + 4].position.y = sin(lat + kLatEvery);
			vertexDate[start + 4].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexDate[start + 4].position.w = 1.0f;
			vertexDate[start + 4].texcoord = { u + (1.0f / float(kSubdivision_)), v - (1.0f / float(kSubdivision_)) };
			vertexDate[start + 4].normal.x = vertexDate[start + 4].position.x;
			vertexDate[start + 4].normal.y = vertexDate[start + 4].position.y;
			vertexDate[start + 4].normal.z = vertexDate[start + 4].position.z;
			indexData[start + 4] = start + 4;
			//c
			vertexDate[start + 5].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexDate[start + 5].position.y = sin(lat);
			vertexDate[start + 5].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexDate[start + 5].position.w = 1.0f;
			vertexDate[start + 5].texcoord = { u + (1.0f / float(kSubdivision_)), v };
			vertexDate[start + 5].normal.x = vertexDate[start + 5].position.x;
			vertexDate[start + 5].normal.y = vertexDate[start + 5].position.y;
			vertexDate[start + 5].normal.z = vertexDate[start + 5].position.z;
			indexData[start + 5] = start + 5;
		}
	}

	//マテリアル--------------------------------------------------------------------------------------
	materialResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	materialDate_ = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込む
	materialDate_->materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDate_->materialData_->enableLighting = 2;
	//UVTransformは単位行列を書き込んでおく
	materialDate_->materialData_->uvTransform = MakeIdentity4x4();
	//平行光源--------------------------------------------------------------------------------------------------
	directionalLightResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(DirectionalLight));
	//データ書き込む
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//デフォルト値はこうする
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;
	//行列--------------------------------------------------------------------------------------------------------
	wvpResouce_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	wvpDate_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResouce_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
	//単位行列を書き込んでおく
	wvpDate_->WVP = MakeIdentity4x4();
	wvpDate_->World = MakeIdentity4x4();
	//三角形****************************************************************************************
}

#ifdef _DEBUG
void Mesh::DebugImGui() {
	/*ImGui::Begin("Lighting");*/
	ImGui::ColorEdit4(" Color", (float*)&materialDate_->materialData_->color);
	ImGui::DragFloat3("Direction", (float*)&directionalLightData_->direction, 0.01f);
	directionalLightData_->direction = Vector3::Normalize(directionalLightData_->direction);
	ImGui::DragFloat("Intensity", (float*)&directionalLightData_->intensity, 0.1f);
	const char* lightingModes[] = { "No Lighting", "Lambert", "Half Lambert" };
	ImGui::Combo("Lighting Mode", &materialDate_->materialData_->enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));
	/*ImGui::End();*/
}
#endif


void Mesh::DrawSphere(const WorldTransform& worldTransform, const ViewProjection& viewProjection, D3D12_GPU_DESCRIPTOR_HANDLE texture) {
	wvpDate_->WVP = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;

	directXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBV
	//形状を設定
	directXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResouce_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture);
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//描画(DrawCall/ドローコール)
	/*commandList_->DrawInstanced(shpereVertexNum_, 1, 0, 0);*/
	directXCommon->GetCommandList()->DrawIndexedInstanced(shpereVertexNum_, 1, 0, 0, 0);
}

//void Mesh::ReleaseMesh() {
//	vertexResource_->Release();
//	vertexResourceSprite_->Release();
//	indexResourceSprite_->Release();
//	directionalLightResource_->Release();
//	indexResource_->Release();
//	wvpResourceSprite_->Release();
//	materialResource_->Release();
//	materialResourceSprite_->Release();
//	wvpResouce_->Release();
//}