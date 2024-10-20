#include "Model.h"
#include<fstream>
#include<sstream>
#include<assert.h>
#include"DirectXCommon.h"
#include"TextureManager.h"
#include "externals/imgui/imgui.h"
namespace {
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	//Model* model=Model::GetInstance();
}
Model* Model::GetInstance() {
	static Model instance;
	return &instance;
}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;//構築するModelData
	std::vector<Vector4>positions;//位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	VertexData triangle[3];

	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texccord;
			s >> texccord.x >> texccord.y;
			texccord.y = 1.0f - texccord.y;
			texcoords.push_back(texccord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されている、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、次の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texccrd = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
			/*	VertexData vertex = { position,texccrd,normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position,texccrd,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

MaterialData Model:: LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;//構築するmaterialData
	std::string line;//ファイルから読み込んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

void Model::CreateModel() {
	modelData_ = LoadObjFile("resources", "plane.obj");
	//頂点リソースをつくる
	vertexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファビューを作成する
	vertexBufferView_ = {};
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	VertexData* vertexDate = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));
	std::memcpy(vertexDate, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//マテリアル--------------------------------------------------------------------------------------
	materialResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));
	//マテリアルにデータを書き込む
	materialDate_ = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込む
	materialDate_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDate_->enableLighting = true;
	//UVTransformは単位行列を書き込んでおく
	materialDate_->uvTransform = MakeIdentity4x4();
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
	wvpResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	wvpDate_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
	//単位行列を書き込んでおく
	wvpDate_->WVP = MakeIdentity4x4();
	wvpDate_->World = MakeIdentity4x4();
	//三角形****************************************************************************************

}
#ifdef _DEBUG
void Model::DebugImGui() {
	ImGui::Begin("useMonsterBall");
	ImGui::Checkbox("useMonsterBall", &useMonsterBall);
	ImGui::End();
	ImGui::Begin("Lighting");
	ImGui::ColorEdit4(" Color", (float*)&directionalLightData_->color);
	ImGui::DragFloat3("Direction", (float*)&directionalLightData_->direction, 0.01f);
	directionalLightData_->direction = Normnalize(directionalLightData_->direction);
	ImGui::DragFloat("Intensity", (float*)&directionalLightData_->intensity, 0.1f);
	ImGui::End();
}
#endif

void Model::DrawModel() {
	directXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBV
	//形状を設定
	directXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? TextureManager::GetInstance()->GetTextureSrvHandleGPU2() : TextureManager::GetInstance()->GetTextureSrvHandleGPU());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//描画(DrawCall/ドローコール)
	/*commandList_->DrawInstanced(shpereVertexNum_, 1, 0, 0);*/
	directXCommon->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

