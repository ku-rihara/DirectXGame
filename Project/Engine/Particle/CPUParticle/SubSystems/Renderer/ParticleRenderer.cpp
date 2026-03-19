#include "ParticleRenderer.h"

using namespace KetaEngine;

#include "Base/Dx/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Base/WinApp.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Pipeline/PipelineManager.h"
#include "MathFunction.h"

void ParticleRenderer::AlphaAdapt(
    ParticleFprGPU& data,
    const ParticleManager::Particle& parm,
    const ParticleManager::ParticleGroup& group) {

    data.color = parm.color_;
    if (group.param.isAlphaNoMove) {
        data.color.w = 1.0f;
        return;
    }
    data.color.w = 1.0f - (parm.currentTime_ / parm.lifeTime_);
}

uint32_t ParticleRenderer::BuildInstancingData(
    ParticleManager::ParticleGroup& group,
    ParticleFprGPU* instancingData,
    const Matrix4x4& matView,
    const Matrix4x4& matProj) {

    std::list<ParticleManager::Particle>& particles = group.particles;
    uint32_t instanceIndex = 0;

    for (auto it = particles.begin(); it != particles.end();) {
        if (it->currentTime_ >= it->lifeTime_) {
            it = particles.erase(it);
            continue;
        }
        if (instanceIndex >= group.currentNum) {
            break;
        }

        // WVP適応
        instancingData[instanceIndex].World                 = it->worldTransform_->matWorld_;
        instancingData[instanceIndex].WVP                   = it->worldTransform_->matWorld_ * matView * matProj;
        instancingData[instanceIndex].WorldInverseTranspose = Inverse(Transpose(it->worldTransform_->matWorld_));

        /// Alpha適応
        AlphaAdapt(instancingData[instanceIndex], *it, group);

        ///==========================================================================================
        //  UVTransform
        ///==========================================================================================
        instancingData[instanceIndex].UVTransform = MakeAffineMatrix(it->uvInfo_.scale, it->uvInfo_.rotate, it->uvInfo_.pos);

        instancingData[instanceIndex].isFlipX = it->uvInfo_.isFlipX ? 1u : 0u;
        instancingData[instanceIndex].isFlipY = it->uvInfo_.isFlipY ? 1u : 0u;

        // Dissolveしきい値書き込み
        if (it->isAdaptDissolveEasing && it->dissolveThresholdData_) {
            instancingData[instanceIndex].dissolveThreshold = *it->dissolveThresholdData_;
        } else {
            instancingData[instanceIndex].dissolveThreshold = 1.0f;
        }

        ++instanceIndex;
        ++it;
    }

    return instanceIndex;
}

void ParticleRenderer::DrawGroup(
    ParticleManager::ParticleGroup& group,
    uint32_t instanceIndex,
    SrvManager* srvManager,
    ID3D12GraphicsCommandList* commandList) {

    PipelineManager::GetInstance()->PreDraw(PipelineType::Particle, commandList);
    PipelineManager::GetInstance()->PreBlendSet(PipelineType::Particle, commandList, group.param.blendMode);

    // マテリアルのリソースを設定
    group.material.SetCommandList(commandList);
    commandList->SetGraphicsRootDescriptorTable(
        static_cast<UINT>(ParticleRootParameter::ParticleData),
        srvManager->GetGPUDescriptorHandle(group.srvIndex));

    // テクスチャハンドルの設定
    commandList->SetGraphicsRootDescriptorTable(
        static_cast<UINT>(ParticleRootParameter::Texture),
        TextureManager::GetInstance()->GetTextureHandle(group.textureHandle));

    // ディゾルブテクスチャの設定
    uint32_t dTexHandle = (group.dissolveTextureHandle != 0) ? group.dissolveTextureHandle : group.textureHandle;
    commandList->SetGraphicsRootDescriptorTable(
        static_cast<UINT>(ParticleRootParameter::DissolveTexture),
        TextureManager::GetInstance()->GetTextureHandle(dTexHandle));

    // モデル描画
    if (group.model) {
        group.model->DrawInstancing(instanceIndex);
        // メッシュ描画
    } else if (group.primitive_->GetMesh()) {
        group.primitive_->GetMesh()->DrawInstancing(instanceIndex);
    }
}

void ParticleRenderer::Draw(
    std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
    const ViewProjection& viewProjection,
    SrvManager* srvManager) {

    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

    for (auto& groupPair : groups) {
        ParticleManager::ParticleGroup& group = groupPair.second;
        if (group.param.isScreenPos) {
            continue; // DrawScreenPos() で描画
        }

        ParticleFprGPU* instancingData = group.instancingData;

        uint32_t instanceIndex = BuildInstancingData(
            group,
            instancingData,
            viewProjection.matView_,
            viewProjection.matProjection_);

        if (instanceIndex > 0) {
            DrawGroup(group, instanceIndex, srvManager, commandList);
        }
    }
}

void ParticleRenderer::DrawScreenPos(
    std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
    SrvManager* srvManager) {

    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

    // スプライトと同じ正射影行列
    Matrix4x4 matProj = MakeOrthographicMatrix(
        0.0f, 0.0f,
        float(WinApp::kWindowWidth), float(WinApp::kWindowHeight),
        0.0f, 100.0f);
    Matrix4x4 matView = MakeIdentity4x4();

    for (auto& groupPair : groups) {
        ParticleManager::ParticleGroup& group = groupPair.second;
        if (!group.param.isScreenPos) {
            continue;
        }

        ParticleFprGPU* instancingData = group.instancingData;

        uint32_t instanceIndex = BuildInstancingData(
            group,
            instancingData,
            matView,
            matProj);

        if (instanceIndex > 0) {
            DrawGroup(group, instanceIndex, srvManager, commandList);
        }
    }
}
