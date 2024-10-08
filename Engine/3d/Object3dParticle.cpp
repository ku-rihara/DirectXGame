#include "Object3dParticle.h"
#include "ImGuiManager.h"
#include "DirectXCommon.h"
#include "ModelManager.h"


Object3dParticle* Object3dParticle::CreateModel(const std::string& instanceName, const std::string& extension, const uint32_t& instanceNumMax) {
	Object3dParticle* object3d = new Object3dParticle();
	object3d->instanceMax_ = instanceNumMax;

	ModelManager::GetInstance()->LoadModelParticle(instanceName, extension);
	object3d->SetModel(instanceName, extension);
	object3d->CreateInstancingResource(instanceNumMax);
	object3d->Clear();
	return object3d;
}

void Object3dParticle::Update(std::optional<const ViewProjection*> viewProjection) {
	for (std::list<Particle>::iterator particleIterator = particles_.begin();
		particleIterator != particles_.end(); ++particleIterator) {
		(*particleIterator).worldTransform_.translation_ += (*particleIterator).velocity_*kDeltaTime_;
		if (viewProjection.has_value()) {
			(*particleIterator).worldTransform_.BillboardUpdateMatrix(*viewProjection.value());
		}
		else {
			(*particleIterator).worldTransform_.UpdateMatrix();
		}

		// パーティクルの時間を更新
		(*particleIterator).currentTime_ += kDeltaTime_;
	}
}

void Object3dParticle::Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
	instanceNum_ = 0; // 描画するインスタンスの数をリセット

	for (auto particleIterator = particles_.begin(); particleIterator != particles_.end(); /* ++particleIterator はここに置かない */) {
		if ((*particleIterator).currentTime_ >= (*particleIterator).lifeTime_) {
			particleIterator = particles_.erase(particleIterator); // erase の戻り値でイテレータを更新
			continue; // 生存時間を過ぎたパーティクルは描画しない
		}

		// WVP行列の計算
		float alpha = 1.0f - ((*particleIterator).currentTime_ / (*particleIterator).lifeTime_); // アルファ値の計算
		if (instanceNum_ < instanceMax_) {
			// WVP行列の計算
			if (model_->GetIsFileGltf()) {
				instancingData_[instanceNum_].WVP = model_->GetModelData().rootNode.localMatrix * (*particleIterator).worldTransform_.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
				instancingData_[instanceNum_].WorldInverseTranspose = Inverse(Transpose(model_->GetModelData().rootNode.localMatrix * (*particleIterator).worldTransform_.matWorld_));
			}
			else {
				instancingData_[instanceNum_].WVP = (*particleIterator).worldTransform_.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
				instancingData_[instanceNum_].WorldInverseTranspose = Inverse(Transpose((*particleIterator).worldTransform_.matWorld_));
			}

			instancingData_[instanceNum_].color = (*particleIterator).color_;
			instancingData_[instanceNum_].color.w = alpha; // アルファ値を反映
			++instanceNum_;
		}

		++particleIterator; // ここでインクリメント
	}

	if (model_) {
		model_->DrawParticle(instanceNum_, instancingSrvHandleGPU_, textureHandle);
	}
}

void Object3dParticle::DebugImgui() {
	BaseObject3d::DebugImgui();
}

void Object3dParticle::CreateInstancingResource(const uint32_t& instanceNum) {
	instanceNum_ = instanceNum;

	//Instancing用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
		DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * instanceNum_);

	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	for (uint32_t index = 0; index < instanceNum_; ++index) {
		instancingData_[index].WVP = MakeIdentity4x4();
		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceNum_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleFprGPU);

	instancingSrvHandleCPU_ = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(
		ImGuiManager::GetInstance()->GetSrvDescriptorHeap(), DirectXCommon::GetInstance()->GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU_ = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(
		ImGuiManager::GetInstance()->GetSrvDescriptorHeap(), DirectXCommon::GetInstance()->GetDescriptorSizeSRV(), 3);

	instancingResources_.push_back(instancingResource.Get());

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
}

void Object3dParticle::Clear() {
	
	particles_.clear(); // リストをクリア
	

}

//パーティクル作成
Object3dParticle::Particle  Object3dParticle::MakeParticle(std::uniform_real_distribution<float> dist, std::uniform_real_distribution<float>velocityDist,const Transform&transform, float lifeTime) {
	
	Particle particle;
	particle.lifeTime_ = lifeTime;
	//初期化
	particle.worldTransform_.Init();
	//回転
	particle.worldTransform_.rotation_.y = -3.0f;
	//色
	particle.color_ = { dist(random), dist(random), dist(random), 1.0f };
	//座標
	Vector3 randomTranslate= { dist(random), dist(random), dist(random) };
	particle.worldTransform_.translation_ = transform.translate + randomTranslate;
	//速度
	particle.velocity_ = { velocityDist(random), velocityDist(random), 0 };
	
	return  particle;
}

void Object3dParticle::Emit(const Emitter& emitter, std::uniform_real_distribution<float> dist, std::uniform_real_distribution<float>velocityDist, float lifeTime) {
	
	for (uint32_t i = 0; i < emitter.count; ++i) {
		particles_.emplace_back(MakeParticle(dist, velocityDist,emitter.transform, lifeTime));
	}
	//for (uint32_t i = 0; i < emitter.count; ++i) {
	//	Particle particle = MakeParticle(dist, velocityDist, lifeTime);
	//	particles_.push_back(particle); // emittedParticles に追加
	//}
	/*return particles;*/
}
