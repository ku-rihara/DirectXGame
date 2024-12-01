#include"ParticleManager.h"
#include"ParticleCommon.h"
#include"3d/ModelManager.h"
//frame
#include"Frame/Frame.h"

//Function
#include"function/Log.h"
#include<cassert>
#include<string>


ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}


void ParticleManager::Init(SrvManager* srvManager) {
	pSrvManager_ = srvManager;
	pParticleCommon_ = ParticleCommon::GetInstance();
}


void ParticleManager::Update(std::optional<const ViewProjection*> viewProjection) {
	// 各粒子グループを周る
	for (std::unordered_map<std::string, ParticleGroup>::iterator groupIt = particleGroups_.begin();
		groupIt != particleGroups_.end(); ++groupIt) {

		ParticleGroup& group = groupIt->second;
		std::list<Particle>& particles = group.particles;
		ParticleFprGPU* instancingData = group.instancingData;

		uint32_t instanceIndex = 0; // 現在のインスタンス数

		for (std::list<Particle>::iterator it = particles.begin(); it != particles.end();) {
			// パーティクルの寿命をチェック
			if (it->currentTime_ >= it->lifeTime_) {
				it = particles.erase(it); // 寿命が尽きた場合、リストから削除
				continue;
			}

			// 加速度フィールドの適用
			if (accelerationField_.isAdaption &&
				IsCollision(accelerationField_.area, it->worldTransform_.translation_)) {
				it->velocity_ += accelerationField_.acceleration * Frame::DeltaTime();
			}

			// パーティクルの位置更新
			it->worldTransform_.translation_ += it->velocity_ * Frame::DeltaTime();

			// ビルボードまたは通常の行列更新
			if (viewProjection.has_value()) {
				it->worldTransform_.BillboardUpdateMatrix(*viewProjection.value());
			}
			else {
				it->worldTransform_.UpdateMatrix();
			}

			// 時間を進める
			it->currentTime_ += Frame::DeltaTime();
			++it;
		}

		// グループのインスタンス数を更新
		group.instanceNum = instanceIndex;
	}
}



void ParticleManager::Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
	// 各粒子グループを周る
	for (std::unordered_map<std::string, ParticleGroup>::iterator groupIt = particleGroups_.begin();
		groupIt != particleGroups_.end(); ++groupIt) {

		ParticleGroup& group = groupIt->second;
		const std::list<Particle>& particles = group.particles;
		ParticleFprGPU* instancingData = group.instancingData;

		uint32_t instanceIndex = 0; // 描画するインスタンス数

		for (std::list<Particle>::const_iterator it = particles.begin(); it != particles.end(); ++it) {
			// インスタンシングデータを設定
			if (instanceIndex < group.instanceNum) {
				// WVP行列の計算
				instancingData[instanceIndex].WVP = it->worldTransform_.matWorld_ *
					viewProjection.matView_ * viewProjection.matProjection_;

				// 法線用の逆転置行列を計算
				instancingData[instanceIndex].WorldInverseTranspose =
					Inverse(Transpose(it->worldTransform_.matWorld_));

				// カラー設定（アルファ値を減少させる）
				instancingData[instanceIndex].color = it->color_;
				instancingData[instanceIndex].color.w = 1.0f - (it->currentTime_ / it->lifeTime_);

				++instanceIndex;
			}
		}

		// 描画処理を呼び出す
		if (group.model) {
			group.model->DrawParticle(instanceIndex,
				pSrvManager_->GetGPUDescriptorHandle(group.srvIndex),
				group.material,
				textureHandle);
		}
	}
}


void ParticleManager::CreateParticleGroup(const std::string name,const std::string modelFilePath, const std::string& extension,const uint32_t& maxnum) {
		if (particleGroups_.contains(name)) {
		return;
	}

		// グループ追加
	particleGroups_[name] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups_[name];

	/// モデル
	ModelManager::GetInstance()->LoadModel(modelFilePath, extension);
	SetModel(name,modelFilePath, extension);

	/// リソース作成
	CreateInstancingResource(name,maxnum);//インスタンシング
	CreateMaterialResource(name);// マテリアル

	particleGroup.instanceNum = 0;
}


void  ParticleManager::SetModel(const std::string& name,const std::string& modelName, const std::string& extension) {

	//モデルを検索してセット
	particleGroups_[name].model = (ModelManager::GetInstance()->FindModel(modelName, extension));
}

void  ParticleManager::CreateMaterialResource(const std::string& name) {
	particleGroups_[name].material.CreateMaterialResource(DirectXCommon::GetInstance());
}

//
//void Object3dParticle::DebugImgui() {
//	BaseObject3d::DebugImgui();
//}

void ParticleManager::CreateInstancingResource(const std::string& name, const uint32_t& instanceNum) {
	particleGroups_[name].instanceNum = instanceNum;

	//Instancing用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
		DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * instanceNum);

	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups_[name].instanceNum));

	for (uint32_t index = 0; index < particleGroups_[name].instanceNum; ++index) {
		particleGroups_[name].instancingData[index].WVP = MakeIdentity4x4();
		particleGroups_[name].instancingData[index].World = MakeIdentity4x4();
		particleGroups_[name].instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	// SRV確保
	particleGroups_[name].srvIndex = pSrvManager_->Allocate();

	// SRVの作成
	pSrvManager_->CreateSRVforStructuredBuffer(
		particleGroups_[name].srvIndex,
		particleGroups_[name].instancingResource.Get(),
		instanceNum,
		sizeof(ParticleFprGPU)
	);
}

//void Object3dParticle::Clear() {
//
//	particles_.clear(); // リストをクリア
//
//}

////パーティクル作成
//Object3dParticle::Particle  Object3dParticle::MakeParticle(MinMax dist, MinMax velocityDist, const Transform& transform, float lifeTime) {
//
//	Particle particle;
//	particle.lifeTime_ = lifeTime;
//	//初期化
//	particle.worldTransform_.Init();
//	//回転
//	particle.worldTransform_.rotation_.y = -3.0f;
//	//色
//	particle.color_ = { Random::Range(dist.min,dist.max),  Random::Range(dist.min,dist.max),  Random::Range(dist.min,dist.max), 1.0f };
//	//座標
//	Vector3 randomTranslate = { Random::Range(dist.min,dist.max),  Random::Range(dist.min,dist.max),  Random::Range(dist.min,dist.max) };
//	particle.worldTransform_.translation_ = transform.translate + randomTranslate;
//	//速度
//	particle.velocity_ = { Random::Range(velocityDist.min,velocityDist.max), Random::Range(velocityDist.min,velocityDist.max), 0 };
//
//	return  particle;
//}

////エミッター
//void ParticleManager::Emit(const Emitter& emitter, MinMax dist, MinMax velocityDist, float lifeTime) {
//
//	for (uint32_t i = 0; i < emitter.count; ++i) {
//		particles_.emplace_back(MakeParticle(dist, velocityDist, emitter.transform, lifeTime));
//	}
//}

