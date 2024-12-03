#include"ParticleManager.h"
#include"ParticleCommon.h"
#include"3d/ModelManager.h"
//frame
#include"Frame/Frame.h"

//Function
#include"random.h"
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
	//uint32_t instanceIndex = 0; // 現在のインスタンス数

	// 各粒子グループを周る
	for (auto& groupPair : particleGroups_) {
		ParticleGroup& group = groupPair.second;
		std::list<Particle>& particles = group.particles;

		// 粒子リストを更新
		for (auto it = particles.begin(); it != particles.end();) {
			

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
	/*	group.instanceNum = instanceIndex;*/
	}
}



void ParticleManager::Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
	uint32_t instanceIndex = 0; // 描画するインスタンス数

	// 各粒子グループを周る
	for (auto& groupPair : particleGroups_) {
		ParticleGroup& group = groupPair.second;
		std::list<Particle>& particles = group.particles;
		ParticleFprGPU* instancingData = group.instancingData;

		// 各粒子のインスタンシングデータを設定
		for (auto it = particles.begin(); it != particles.end();) {
			// パーティクルの寿命をチェック
			if (it->currentTime_ >= it->lifeTime_) {
				it = particles.erase(it); // 寿命が尽きた場合、リストから削除
				continue; // 次の粒子へ
			}

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
			++it;
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



void ParticleManager::CreateParticleGroup(
	const std::string name, const std::string modelFilePath,
	const std::string& extension, const uint32_t& maxnum) {
	if (particleGroups_.contains(name)) {
		return;
	}

	// グループ追加
	particleGroups_[name] = ParticleGroup();


	/// モデル
	ModelManager::GetInstance()->LoadModel(modelFilePath, extension);
	SetModel(name, modelFilePath, extension);

	/// リソース作成
	CreateInstancingResource(name, maxnum);//インスタンシング
	CreateMaterialResource(name);// マテリアル

	/*particleGroup.instanceNum = 0;*/
}


void  ParticleManager::SetModel(const std::string& name, const std::string& modelName, const std::string& extension) {

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
	particleGroups_[name].instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
		DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * particleGroups_[name].instanceNum);

	particleGroups_[name].instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups_[name].instancingData));

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


///======================================================================
/// パーティクル作成
///======================================================================
ParticleManager::Particle ParticleManager::MakeParticle(
	const Vector3& basePosition, V3MinMax positionDist,
	V3MinMax scaledist, V3MinMax velocityDist, const Vector4& baseColor,
	const V4MinMax& colorDist, float lifeTime) {

	Particle particle;
	particle.lifeTime_ = lifeTime;
	particle.currentTime_ = 0.0f;
	//初期化
	particle.worldTransform_.Init();
	//回転
	particle.worldTransform_.rotation_.y = -3.0f;
	/// スケール
	particle.worldTransform_.scale_ = { Random::Range(scaledist.min.x,scaledist.max.x),  Random::Range(scaledist.min.y,scaledist.max.y),  Random::Range(scaledist.min.z,scaledist.max.z) };
	//座標
	Vector3 randomTranslate = { Random::Range(positionDist.min.x,positionDist.max.x),  Random::Range(positionDist.min.y,positionDist.max.y),  Random::Range(positionDist.min.z,positionDist.max.z) };
	particle.worldTransform_.translation_ = basePosition + randomTranslate;
	//速度
	particle.velocity_ = { Random::Range(velocityDist.min.x,velocityDist.max.x), Random::Range(velocityDist.min.y,velocityDist.max.y), Random::Range(velocityDist.min.z,velocityDist.max.z) };

	//色
	Vector4 randomColor{ 
		Random::Range(colorDist.min.x,colorDist.max.x), 
		Random::Range(colorDist.min.y,colorDist.max.y), 
		Random::Range(colorDist.min.z,colorDist.max.z), 
		0.0f };

	particle.color_ = baseColor + randomColor;

	return  particle;
}

///======================================================================
/// エミット
///======================================================================
void ParticleManager::Emit(
	std::string name, const Vector3& basePosition, V3MinMax positionDist,
	V3MinMax scaledist, V3MinMax velocityDist, const Vector4& baseColor,
	const V4MinMax& colorDist,float lifeTime, uint32_t count) {

	// パーティクルグループが存在するか確認
	assert(particleGroups_.find(name) != particleGroups_.end() && "Error: パーティクルグループがありません。");

	// 指定されたパーティクルグループを取得
	ParticleGroup& particleGroup = particleGroups_[name];

	// 生成、グループ追加
	std::list<Particle> particles;
	for (uint32_t i = 0; i < count; ++i) {
		particles.emplace_back(MakeParticle(basePosition, positionDist, scaledist, velocityDist,baseColor, colorDist, lifeTime));
	}

	// グループに追加
	particleGroup.particles.splice(particleGroup.particles.end(), particles);
}

