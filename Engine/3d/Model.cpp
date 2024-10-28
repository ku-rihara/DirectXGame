#include "Model.h"
#include<fstream>
#include<sstream>
#include<assert.h>
#include <imgui.h>

//class
#include"Light.h"
#include"base/Object3DCommon.h"
#include"2d/ImGuiManager.h"
#include"base/TextureManager.h"
namespace {
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
}

void ModelCommon::Init(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;//構築するModelData
	std::vector<Vector4>positions;//位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	VertexData triangle[3];

	Assimp::Importer importer;
	std::string filePath(directoryPath + "/" + filename);//ファイルを開く
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないmeshは非対応
		//assert(mesh->HasTextureCoords(0));//Texcoordがないmeshは非対応

		//meshの中身faceの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices = 3);//三角形のみサポート

			//vertexを解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y ,position.z,1.0f };
				vertex.normal = { normal.x,normal.y ,normal.z };
				if (mesh->HasTextureCoords(0)) {
					vertex.texcoord = { texcoord.x,texcoord.y };
				}
				//右手→左手に変換する
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
			}
		}
	}
	//Materialを解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}
	//テクスチャが無ければ
	if (modelData.material.textureFilePath.empty()) {
		std::string whiteTexture = "default.png";
		modelData.material.textureFilePath = "./Resources/Model/" + whiteTexture;

	}
	return modelData;
}

ModelData 	Model::LoadModelGltf(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;//構築するModelData
	Assimp::Importer importer;
	std::string filePath(directoryPath + "/" + filename);//ファイルを開く
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	modelData = LoadModelFile(directoryPath, filename);
	modelData.rootNode = ReadNode(scene->mRootNode);
	return modelData;
}
Node Model::ReadNode(aiNode* node) {
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;//nodeのlocalMatrix
	aiLocalMatrix.Transpose();//転置
	result.localMatrix.m[0][0] = aiLocalMatrix[0][0];//他の要素も同様に

	result.name = node->mName.C_Str();//Node名を格納
	result.cihldren.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰的に読んで階層構造を作っていく
		result.cihldren[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

void Model::CreateModel(const std::string& ModelName, const std::string& extension) {
	if (extension == ".gltf") {
		isFileGltf_ = true;
		modelData_ = LoadModelGltf("./Resources/Model/" + ModelName, ModelName + extension);
	}
	else {
		modelData_ = LoadModelFile("./Resources/Model/" + ModelName, ModelName + extension);
	}
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
	
	//material_.textureFilePath = modelData_.material.textureFilePath;
	
}

//マテリアルリソース作成
void Model::CreateMaterialResource() {
	material_.CreateMaterialResource(directXCommon);	
	Light::GetInstance()->Init();
}


void Model::DebugImGui() {
#ifdef _DEBUG
	material_.DebugImGui();
#endif
}


void Model::Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ObjectColor& color, std::optional<uint32_t> textureHandle){

	auto commandList = directXCommon->GetCommandList();
	/*materialDate_->color = color.;*/

	// 頂点バッファとインデックスバッファの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//commandList->IASetIndexBuffer(&indexBufferView_);  // IBV

	// 形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	color;
	/*material_.UpdateMaterialData(color.GetColor());*/
	material_.SetCommandList(commandList);
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

void Model::DrawParticle(const uint32_t instanceNum, D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle,
	std::optional<uint32_t> textureHandle) {
	auto commandList = directXCommon->GetCommandList();

	// ルートシグネチャとパイプラインステートを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルのリソースを設定
	material_.SetCommandList(commandList);
	commandList->SetGraphicsRootDescriptorTable(1, instancingGUPHandle);

	// テクスチャハンドルの設定
	if (textureHandle.has_value()) {
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
	}
	else {
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	}

	commandList->DrawInstanced(UINT(modelData_.vertices.size()), instanceNum, 0, 0);

}

void  Model::PreDraw(ID3D12GraphicsCommandList* commandList) {
	Object3DCommon::GetInstance()->PreDraw(commandList);
}

void  Model::PreDrawParticle(ID3D12GraphicsCommandList* commandList) {
	Object3DCommon::GetInstance()->PreDrawParticle(commandList);
}

