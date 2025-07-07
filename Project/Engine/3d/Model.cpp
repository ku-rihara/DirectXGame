#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
//
// class
#include "base/SkyBoxRenderer.h"
#include "base/TextureManager.h"
#include "Lighrt/Light.h"
#include "Pipeline/Object3DPiprline.h"
#include <filesystem>

void ModelCommon::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    Light::GetInstance()->Init(dxCommon_->GetDevice());
}

ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
    ModelData modelData; // 構築するModelData
    std::vector<Vector4> positions; // 位置
    std::vector<Vector3> normals; // 法線
    std::vector<Vector2> texcoords; // テクスチャ座標
    std::string line; // ファイルから読んだ1行を格納するもの
    VertexData triangle[3];

    Assimp::Importer importer;
    std::string filePath(directoryPath + "/" + filename); // ファイルを開く
    const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
    assert(scene->HasMeshes()); // メッシュがないのは対応しない

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals()); // 法線がないmeshは非対応

        modelData.vertices.resize(mesh->mNumVertices);
        for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
            aiVector3D& position = mesh->mVertices[vertexIndex];
            aiVector3D& normal   = mesh->mNormals[vertexIndex];
            aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

            modelData.vertices[vertexIndex].position = {-position.x, position.y, position.z, 1.0f};
            modelData.vertices[vertexIndex].normal   = {-normal.x, normal.y, normal.z};
            modelData.vertices[vertexIndex].texcoord = {texcoord.x, texcoord.y};
        }


          // meshの中身faceの解析
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices = 3); // 三角形のみサポート

            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIndex = face.mIndices[element];
                modelData.indices.push_back(vertexIndex);
            }
        }


        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            aiBone* bone                     = mesh->mBones[boneIndex];
            std::string jointName            = bone->mName.C_Str();
            JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

            aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
            aiVector3D scale, translate;
            aiQuaternion rotate;
            bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
            // affine
            Matrix4x4 bindPoseMatrix = MakeAffineMatrixQuaternion(
                Vector3(scale.x, scale.y, scale.z),
                Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w),
                Vector3(-translate.x, translate.y, translate.z));

            jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

            for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
                jointWeightData.vertexWeights.push_back(
                    {bone->mWeights[weightIndex].mWeight,
                        bone->mWeights[weightIndex].mVertexId});
            }
        }

      
    }

    // Materialを解析
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
            modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
        }
    }
    // テクスチャが無ければ
    if (modelData.material.textureFilePath.empty()) {
        std::string whiteTexture           = "default.png";
        modelData.material.textureFilePath = texturePath_ + whiteTexture;
    }
    return modelData;
}

ModelData Model::LoadModelGltf(const std::string& directoryPath, const std::string& filename) {
    ModelData modelData; // 構築するModelData
    Assimp::Importer importer;
    std::string filePath(directoryPath + "/" + filename); // ファイルを開く
    const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
    modelData            = LoadModelFile(directoryPath, filename);
    modelData.rootNode   = ReadNode(scene->mRootNode);
    return modelData;
}

Node Model::ReadNode(aiNode* node) {
    aiVector3D scale, translate;
    aiQuaternion rotate;
    Node result;

    node->mTransformation.Decompose(scale, rotate, translate);
    result.transform.scale     = {scale.x, scale.y, scale.z};
    result.transform.rotate    = {rotate.x, -rotate.y, -rotate.z, rotate.w};
    result.transform.translate = {-translate.x, translate.y, translate.z};
    result.localMatrix         = MakeAffineMatrixQuaternion(result.transform.scale, result.transform.rotate, result.transform.translate);
    result.name                = node->mName.C_Str(); // Node名を格納

    // Node result;
    // aiMatrix4x4 aiLocalMatrix = node->mTransformation; // nodeのlocalMatrix
    // aiLocalMatrix.Transpose(); // 転置
    // result.localMatrix.m[0][0] = aiLocalMatrix[0][0]; // 他の要素も同様に

    result.cihldren.resize(node->mNumChildren); // 子供の数だけ確保
    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        // 再帰的に読んで階層構造を作っていく
        result.cihldren[childIndex] = ReadNode(node->mChildren[childIndex]);
    }
    return result;
}

void Model::CreateModel(const std::string& ModelFileName) {
    std::filesystem::path path(ModelFileName);
    std::string extension = path.extension().string();
    std::string stemName  = path.stem().string();

    if (extension == ".gltf") {
        isFileGltf_ = true;
        modelData_  = LoadModelGltf(modelPath_ + stemName, ModelFileName);
    } else {
        modelData_ = LoadModelFile(modelPath_ + stemName, ModelFileName);
    }

    textureManager_ = TextureManager::GetInstance();
    textureHandle_  = textureManager_->LoadTexture(modelData_.material.textureFilePath);

    dxCommon_ = DirectXCommon::GetInstance();

    vertexResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        static_cast<UINT>(sizeof(VertexData) * modelData_.vertices.size()));

    vertexBufferView_                = {};
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes    = static_cast<UINT>(sizeof(VertexData) * modelData_.vertices.size());
    vertexBufferView_.StrideInBytes  = sizeof(VertexData);

    VertexData* vertexData = nullptr;
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

    // indexResource の作成
    indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), static_cast<UINT>(sizeof(uint32_t) * modelData_.indices.size()));

    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes    = static_cast<UINT>(sizeof(uint32_t) * modelData_.indices.size());
    indexBufferView_.Format         = DXGI_FORMAT_R32_UINT;

    uint32_t* indexData = nullptr;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    std::memcpy(indexData, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
}

void Model::DebugImGui() {
#ifdef _DEBUG

#endif
}

void Model::Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, Material material, std::optional<uint32_t> textureHandle) {

    auto commandList = dxCommon_->GetCommandList();
    /*materialDate_->color = color.;*/

    // 頂点バッファとインデックスバッファの設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->IASetIndexBuffer(&indexBufferView_); // IBV

    // 形状を設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    material.SetCommandList(commandList);

    commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

    if (textureHandle.has_value()) {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
    } else {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
    }

    uint32_t environmentalMapTexture = SkyBoxRenderer::GetInstance()->GetEnvironmentalMapTextureHandle();
    commandList->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTextureHandle(environmentalMapTexture));

    Light::GetInstance()->SetLightCommands(commandList);

    // 描画コール
    commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Model::DrawAnimation(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, Material material, SkinCluster skinCluster, std::optional<uint32_t> textureHandle) {

    auto commandList = dxCommon_->GetCommandList();

    D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
        vertexBufferView_, // 頂点データ
        skinCluster.influenceBufferView // インフルエンス
    };

    // 頂点バッファとインデックスバッファの設定
    commandList->IASetIndexBuffer(&indexBufferView_);
    commandList->IASetVertexBuffers(0, 2, vbvs);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    material.SetCommandList(commandList);

    // 定数バッファ（WVPなど）
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

    // テクスチャ
    if (textureHandle.has_value()) {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
    } else {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
    }

    // 環境マップ
    uint32_t environmentalMapTexture = SkyBoxRenderer::GetInstance()->GetEnvironmentalMapTextureHandle();
    commandList->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTextureHandle(environmentalMapTexture));

    commandList->SetGraphicsRootDescriptorTable(10, skinCluster.paletteSrvHandle.second);

    // ライト
    Light::GetInstance()->SetLightCommands(commandList);

    // 描画
    commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Model::DrawInstancing(const uint32_t instanceNum, D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle, Material material,
    std::optional<uint32_t> textureHandle) {
    auto commandList = dxCommon_->GetCommandList();

    // ルートシグネチャとパイプラインステートを設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->IASetIndexBuffer(&indexBufferView_);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアルのリソースを設定
    material.SetCommandList(commandList);
    commandList->SetGraphicsRootDescriptorTable(1, instancingGUPHandle);

    // テクスチャハンドルの設定
    if (textureHandle.has_value()) {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
    } else {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
    }

    commandList->DrawInstanced(UINT(modelData_.vertices.size()), instanceNum, 0, 0);
}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList) {
    Object3DPiprline::GetInstance()->PreDraw(commandList);
}
