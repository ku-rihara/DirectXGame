#include "Model.h"
#include<fstream>
#include<sstream>
#include<assert.h>
#include <imgui.h>
//class
#include"Light.h"
#include"Object3DCommon.h"
#include"ImGuiManager.h"
#include"TextureManager.h"
namespace {
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
}

void ModelCommon::Init(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

Model* Model::Create(const std::string& instanceName) {
	// 新しいModelインスタンスを作成
	Model* model = new Model();
	model->CreateModel(instanceName);
	return model;  // 成功した場合は新しいモデルを返す
}

Model* Model::CreateParticle(const std::string& instanceName, const uint32_t& instanceNum, std::mt19937& randomEngine, std::uniform_real_distribution<float> dist) {
	// 新しいModelインスタンスを作成
	Model* model = new Model();
	model->CreateModelParticle(instanceName,instanceNum,randomEngine,dist);
	return model;  // 成功した場合は新しいモデルを返す
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
				uint32_t elementIndices[3] = { 0, 0, 0 };

				std::string index;
				for (int32_t element = 0; element < 3 && std::getline(v, index, '/'); ++element) {
					if (!index.empty()) {
						elementIndices[element] = std::stoi(index);
					}
				}
				//要素へのIndexから、次の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = { 0.0f, 0.0f };
				if (elementIndices[1] > 0) {
					texcoord = texcoords[elementIndices[1] - 1];
				}
				Vector3 normal = { 0.0f, 0.0f, 0.0f };
				if (elementIndices[2] > 0) {
					normal = normals[elementIndices[2] - 1];
				}
				triangle[faceVertex] = { position, texcoord, normal };
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

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
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
	//テクスチャが無かったら
	if (materialData.textureFilePath.empty())
	{
		std::string whiteTexture = "default.png";
		materialData.textureFilePath = "./Resources/Model/" + whiteTexture;
	}
	return materialData;
}

void Model::CreateCommon(const std::string& ModelName) {
	modelData_ = LoadObjFile("./Resources/Model/" + ModelName, ModelName + ".obj");
	textureManager_ = TextureManager::GetInstance();
	textureHandle_ = textureManager_->LoadTexture(modelData_.material.textureFilePath);

	//頂点リソースをつくる
	vertexResource_ = directXCommon->CreateBufferResource(directXCommon->GetDevice(), (sizeof(VertexData) * modelData_.vertices.size()));
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
	materialDate_->enableLighting = 2;
	//UVTransformは単位行列を書き込んでおく
	materialDate_->uvTransform = MakeIdentity4x4();

	/*materialDate_->hasTexture = useTexture;*/
	Light::GetInstance()->Init();


}

void Model::CreateModel(const std::string& ModelName) {
	CreateCommon(ModelName);
	materialDate_->enableLighting = 2;
	Light::GetInstance()->Init();
}
//	パーティクル
void Model::CreateModelParticle(const std::string& ModelName, const uint32_t& instanceNum, std::mt19937& randomEngine, std::uniform_real_distribution<float> dist) {
	CreateCommon(ModelName);
	//パーティクル数
	instanceNum_ = instanceNum;
	//パーティクル変数初期化
	lifeTimes_.resize(instanceNum);
	currentTimes_.resize(instanceNum);

	//Instancing用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource = directXCommon->CreateBufferResource(directXCommon->GetDevice(), sizeof(ParticleFprGPU) * instanceNum_);
	//書き込む為のアドレスを取得
	instancingData_ = nullptr;
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	//データ初期化
	for (uint32_t index = 0; index < instanceNum_; ++index) {
		instancingData_[index].WVP = MakeIdentity4x4();
		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = Vector4(1.0f,1.0f,1.0f,1.0f);
		lifeTimes_[index] = dist(randomEngine);
	}
	//SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceNum_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleFprGPU);

	instancingSrvHandleCPU_ = directXCommon->GetCPUDescriptorHandle(ImGuiManager::GetInstance()->GetSrvDescriptorHeap(), directXCommon->GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU_ = directXCommon->GetGPUDescriptorHandle(ImGuiManager::GetInstance()->GetSrvDescriptorHeap(), directXCommon->GetDescriptorSizeSRV(), 3);
	instancingResources_.push_back(instancingResource.Get());
	directXCommon->GetDevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
	
}
#ifdef _DEBUG
void Model::DebugImGui() {

	//Material
	ImGui::ColorEdit4(" Color", (float*)&materialDate_->color);
	ImGui::DragFloat("Shininess", (float*)&materialDate_->shininess, 0.01f);
	const char* lightingModes[] = { "No Lighting", "Lambert", "Half Lambert","Specular Reflection","PointLight","SpotLight" };
	ImGui::Combo("Lighting Mode", &materialDate_->enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));
}
#endif

void Model::Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource,std::optional<uint32_t> textureHandle, const Vector4& color) {
	auto commandList = directXCommon->GetCommandList();
	materialDate_->color = color;

	// 頂点バッファとインデックスバッファの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);  // IBV

	// 形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// リソースの設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	if (textureHandle.has_value()) {
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
	}
	else {
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	}

	// 光源のリソース設定
	commandList->SetGraphicsRootConstantBufferView(3, Light::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(4, Light::GetInstance()->GetCameraForGPUResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(5, Light::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(6, Light::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());

	// 描画コール
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

void Model::DrawParticle(const std::vector<std::unique_ptr<WorldTransform>>& worldTransforms, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle, const std::vector<Vector4>& colors) {
	auto commandList = directXCommon->GetCommandList();

	// ルートシグネチャとパイプラインステートを設定

	
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルのリソースを設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	// テクスチャハンドルの設定
	if (textureHandle.has_value()) {
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
	}
	else {
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	}
	
	/*instanceNum_ = 0;*/
	// インスタンシングデータの更新
	for (uint32_t index = 0; index < worldTransforms.size(); ++index) {
		//if (lifeTimes_[index] <= currentTimes_[index]) {//生存時間を過ぎたら描画対象にしない
		//	continue;
		//}
	/*	float alpha = 1.0f - (currentTimes_[index] / lifeTimes_[index]);*/
		instancingData_[index].WVP = worldTransforms[index]->matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
		instancingData_[index].WorldInverseTranspose = Inverse(Transpose(instancingData_[index].World));
		//経過時間を足す
		currentTimes_[index] += kDeltaTime_;
		// 引数がない場合は白色、ある場合は指定された色を設定
		if (index < colors.size() - 1) {
			instancingData_[index].color = colors[index];
			instancingData_[index].color.w = 1.0f;//alpha;
		}
		else {
			instancingData_[index].color = { 1, 1, 1, 1/*alpha*/ }; // デフォルト白色
		}
		++instanceNum_;
	}

	commandList->DrawInstanced(UINT(modelData_.vertices.size()), instanceNum_, 0, 0);

}

void  Model::PreDraw(ID3D12GraphicsCommandList* commandList) {
	Object3DCommon::GetInstance()->PreDraw(commandList);
}

void  Model::PreDrawParticle(ID3D12GraphicsCommandList* commandList) {
	Object3DCommon::GetInstance()->PreDrawParticle(commandList);
}

