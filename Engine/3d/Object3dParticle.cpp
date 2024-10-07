
#include"Object3dParticle.h"
#include"ImGuiManager.h"
#include"DirectXCommon.h"
#include"ModelManager.h"

Object3dParticle* Object3dParticle::CreateModel(const std::string& instanceName,const std::string& extension, const uint32_t& instanceNumMax, std::mt19937& randomEngine, std::uniform_real_distribution<float> dist) {
	// 新しいModelインスタンスを作成
	Object3dParticle* object3d = new Object3dParticle();
	object3d->instanceMax_ = instanceNumMax;
	ModelManager::GetInstance()->LoadModelParticle(instanceName, extension);
	object3d->SetModel(instanceName, extension);
	object3d->CreateInstancingResource(instanceNumMax,randomEngine,dist);
	object3d->SizeSecure(instanceNumMax);

	for (uint32_t index = 0; index < instanceNumMax; ++index) {
		object3d->worldTransforms_[index]->Init();
	}
	
	return object3d;
}


void Object3dParticle::Update(std::optional<const ViewProjection*>viewProjection) {
	for (uint32_t index = 0; index < instanceMax_; ++index) {
		if (viewProjection.has_value()) {//ビルボード
			worldTransforms_[index]->BillboardUpdateMatrix(*viewProjection.value());
		}
		else {
			worldTransforms_[index]->UpdateMatrix();

		}
	}
}

void Object3dParticle::Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
	/*instanceNum_ = 0;*/
	// インスタンシングデータの更新
	for (uint32_t index = 0; index < worldTransforms_.size(); ++index) {
		//if (lifeTimes_[index] <= currentTimes_[index]) {//生存時間を過ぎたら描画対象にしない
		//	continue;
		//}
	/*	float alpha = 1.0f - (currentTimes_[index] / lifeTimes_[index]);*/
		// WVP行列の計算
		if (model_->GetIsFileGltf()) {//.gltfファイルの場合
			instancingData_[index].WVP = model_->GetModelData().rootNode.localMatrix*worldTransforms_[index]->matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
			instancingData_[index].WorldInverseTranspose = Inverse(Transpose(model_->GetModelData().rootNode.localMatrix*instancingData_[index].World));
		}
		instancingData_[index].WVP = worldTransforms_[index]->matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
		instancingData_[index].WorldInverseTranspose = Inverse(Transpose(instancingData_[index].World));
		//経過時間を足す
		currentTimes_[index] += kDeltaTime_;
		
			instancingData_[index].color = colors_[index];
			//instancingData_[index].color.w = 1.0f;//alpha;
	
		++instanceNum_;
	}
	//描画
	if (model_) {
		model_->DrawParticle(instanceNum_,instancingSrvHandleGPU_, textureHandle);
	}

}

void Object3dParticle::DebugImgui() {
	BaseObject3d::DebugImgui();
}

void Object3dParticle::CreateInstancingResource(const uint32_t& instanceNum, std::mt19937& randomEngine, std::uniform_real_distribution<float> dist) {
	//パーティクル数
	instanceNum_ = instanceNum;
	//パーティクル変数初期化
	lifeTimes_.resize(instanceNum);
	currentTimes_.resize(instanceNum);

	//Instancing用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * instanceNum_);
	//書き込む為のアドレスを取得
	instancingData_ = nullptr;
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	//データ初期化
	for (uint32_t index = 0; index < instanceNum_; ++index) {
		instancingData_[index].WVP = MakeIdentity4x4();
		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
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

	instancingSrvHandleCPU_ = DirectXCommon::GetInstance()->GetCPUDescriptorHandle(ImGuiManager::GetInstance()->GetSrvDescriptorHeap(), DirectXCommon::GetInstance()->GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU_ = DirectXCommon::GetInstance()->GetGPUDescriptorHandle(ImGuiManager::GetInstance()->GetSrvDescriptorHeap(), DirectXCommon::GetInstance()->GetDescriptorSizeSRV(), 3);
	instancingResources_.push_back(instancingResource.Get());
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);

}

void Object3dParticle::SizeSecure(const uint32_t& instanceNumMax) {
	//vectorサイズ確保
	worldTransforms_.reserve(instanceNumMax);
	velocities_.reserve(instanceNumMax);
	for (uint32_t i = 0; i < instanceNumMax; ++i) {
		worldTransforms_.emplace_back(std::make_unique<WorldTransform>());
		velocities_.emplace_back(Vector3());
		colors_.emplace_back(Vector4());
	}
}
//ランダム生成
void Object3dParticle::MakeParticle(std::mt19937& random) {
	// PlaneParticle
	std::uniform_real_distribution<float>distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float>alphadistribution(0.0f, 1.0f);
	for (uint32_t index = 0; index < instanceMax_; ++index) {
		worldTransforms_[index]->rotation_.y = -3;
		colors_[index] = { distribution(random), distribution(random), distribution(random), 1.0f };
		worldTransforms_[index]->translation_ = { distribution(random), distribution(random), distribution(random) };
		velocities_[index] = { alphadistribution(random), alphadistribution(random), alphadistribution(random) };
	}
}