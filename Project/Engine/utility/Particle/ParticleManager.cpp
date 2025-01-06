#include"ParticleManager.h"
#include"ParticleCommon.h"
#include"3d/ModelManager.h"
#include"base/TextureManager.h"
//frame
#include"Frame/Frame.h"

//Function
#include"random.h"
#include"MathFunction.h"
#include<cassert>
#include<string>


ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

///============================================================
///　初期化
///============================================================
void ParticleManager::Init(SrvManager* srvManager) {
	pSrvManager_ = srvManager;
	pParticleCommon_ = ParticleCommon::GetInstance();
}

///============================================================
/// 更新
///============================================================
void ParticleManager::Update(std::optional<const ViewProjection*> viewProjection) {
	//uint32_t instanceIndex = 0; // 現在のインスタンス数

	// 各粒子グループを周る
	for (auto& groupPair : particleGroups_) {
		ParticleGroup& group = groupPair.second;
		std::list<Particle>& particles = group.particles;

		///*************************************************
		///パーティクル更新
		///*************************************************
		for (auto it = particles.begin(); it != particles.end();) {


			///------------------------------------------------------------------------
			///加速フィールド
			///------------------------------------------------------------------------
			if (accelerationField_.isAdaption &&
				IsCollision(accelerationField_.area, it->worldTransform_.translation_)) {
				it->velocity_ += accelerationField_.acceleration * Frame::DeltaTimeRate();
			}

			///------------------------------------------------------------------------
			/// 回転させる
			///------------------------------------------------------------------------
			it->worldTransform_.rotation_ += it->rotateSpeed_ * Frame::DeltaTimeRate();

			///------------------------------------------------------------------------
			/// 重力の適用
			///------------------------------------------------------------------------
			it->velocity_.y += it->gravity_ * Frame::DeltaTimeRate();

			///------------------------------------------------------------------------
			/// 変位更新
			///------------------------------------------------------------------------
			it->worldTransform_.translation_ += it->velocity_ * Frame::DeltaTimeRate();

			///------------------------------------------------------------------------
			/// ビルボードまたは通常の行列更新
			///------------------------------------------------------------------------

			if (viewProjection.has_value()) {
				it->worldTransform_.BillboardUpdateMatrix(*viewProjection.value());
			}
			else {
				it->worldTransform_.UpdateMatrix();
			}

			// 時間を進める
			it->currentTime_ += Frame::DeltaTimeRate();
			++it;
		}
	}
}



///============================================================
/// 描画
///============================================================
void ParticleManager::Draw(const ViewProjection& viewProjection) {
	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

    for (auto& groupPair : particleGroups_) {
        ParticleGroup& group = groupPair.second;
        std::list<Particle>& particles = group.particles;
        ParticleFprGPU* instancingData = group.instancingData;

        uint32_t instanceIndex = 0;

        // 各粒子のインスタンシングデータを設定
        for (auto it = particles.begin(); it != particles.end();) {
            if (it->currentTime_ >= it->lifeTime_) {
                it = particles.erase(it);
                continue;
            }

            instancingData[instanceIndex].WVP = it->worldTransform_.matWorld_ *
                viewProjection.matView_ * viewProjection.matProjection_;

            instancingData[instanceIndex].WorldInverseTranspose =
                Inverse(Transpose(it->worldTransform_.matWorld_));

            instancingData[instanceIndex].color = it->color_;
            instancingData[instanceIndex].color.w = 1.0f - (it->currentTime_ / it->lifeTime_);

            ++instanceIndex;
            ++it;
        }
		
        if (instanceIndex > 0 && group.model) {
			ParticleCommon::GetInstance()->PreDraw(commandList,group.blendMode_);
            group.model->DrawParticle(instanceIndex,
                pSrvManager_->GetGPUDescriptorHandle(group.srvIndex),
                group.material,
				group.textureHandle);
        }
    }
}

///============================================================
/// グループ作成
///============================================================
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

	particleGroups_[name].instanceNum = 0;
}

///============================================================
/// テクスチャセット
///============================================================
void ParticleManager::SetTextureHandle(const std::string name, const uint32_t& handle) {
	particleGroups_[name].textureHandle = handle;
}

///============================================================
/// モデルセット
///============================================================

void  ParticleManager::SetModel(const std::string& name, const std::string& modelName, const std::string& extension) {

	//モデルを検索してセット
	ModelManager::GetInstance()->LoadModel(modelName, extension);
	particleGroups_[name].model = (ModelManager::GetInstance()->FindModel(modelName, extension));
	particleGroups_[name].textureHandle = TextureManager::GetInstance()->LoadTexture(
		particleGroups_[name].model->GetModelData().material.textureFilePath);
}

void  ParticleManager::CreateMaterialResource(const std::string& name) {
	particleGroups_[name].material.CreateMaterialResource(DirectXCommon::GetInstance());
}

///============================================================
/// インスタンシングリソース作成
///============================================================
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
	const Vector3& basePosition, const V3MinMax& positionDist,
	const FMinMax& scaledist, const V3MinMax& velocityDist, const Vector4& baseColor,
	const V4MinMax& colorDist, const float& lifeTime, const float& gravity,
	const Vector3& baseRotate, const Vector3& baseRotateSpeed, const V3MinMax& RotateDist,
	const V3MinMax& rotateSpeedDist) {  // 新パラメータ追加

	Particle particle;

	/*particle.blendMode_ = blendmode;*/

	particle.lifeTime_ = lifeTime;
	particle.currentTime_ = 0.0f;
	// 初期化
	particle.worldTransform_.Init();


	///------------------------------------------------------------------------
	///　座標
	///------------------------------------------------------------------------
	Vector3 randomTranslate = {
		Random::Range(positionDist.min.x, positionDist.max.x),
		Random::Range(positionDist.min.y, positionDist.max.y),
		Random::Range(positionDist.min.z, positionDist.max.z)
	};
	particle.worldTransform_.translation_ = basePosition + randomTranslate;

	///------------------------------------------------------------------------
	/// 速度
	///------------------------------------------------------------------------
	particle.velocity_ = {
		Random::Range(velocityDist.min.x, velocityDist.max.x),
		Random::Range(velocityDist.min.y, velocityDist.max.y),
		Random::Range(velocityDist.min.z, velocityDist.max.z)
	};

	///------------------------------------------------------------------------
	/// 回転
	///------------------------------------------------------------------------
	Vector3 rotate = {
		Random::Range(RotateDist.min.x,  RotateDist.max.x),
		Random::Range(RotateDist.min.y,  RotateDist.max.y),
		Random::Range(RotateDist.min.z,  RotateDist.max.z)
	};
	/// Radianに変換
	rotate.x = toRadian(rotate.x);
	rotate.y = toRadian(rotate.y);
	rotate.z = toRadian(rotate.z);

	/// 代入
	particle.worldTransform_.rotation_ = toRadian(baseRotate) + rotate;

	///------------------------------------------------------------------------
	/// 回転スピード
	///------------------------------------------------------------------------
	Vector3 rotateSpeed = {
		Random::Range(rotateSpeedDist.min.x, rotateSpeedDist.max.x),
		Random::Range(rotateSpeedDist.min.y, rotateSpeedDist.max.y),
		Random::Range(rotateSpeedDist.min.z, rotateSpeedDist.max.z)
	};

	/// Radianに変換
	rotateSpeed.x = toRadian(rotateSpeed.x);
	rotateSpeed.y = toRadian(rotateSpeed.y);
	rotateSpeed.z = toRadian(rotateSpeed.z);

	/// 代入
	particle.rotateSpeed_ = toRadian(baseRotateSpeed) + rotateSpeed;

	///------------------------------------------------------------------------
	/// スケール
	///------------------------------------------------------------------------
	
	float scale = Random::Range(scaledist.min, scaledist.max);

	/// 代入
	particle.worldTransform_.scale_ = {
		scale,
		scale,
		scale,
	};

	///------------------------------------------------------------------------
	/// 色
	///------------------------------------------------------------------------
	Vector4 randomColor{
		Random::Range(colorDist.min.x, colorDist.max.x),
		Random::Range(colorDist.min.y, colorDist.max.y),
		Random::Range(colorDist.min.z, colorDist.max.z),
		0.0f
	};
	// 代入
	particle.color_ = baseColor + randomColor;


	/// 重力値
	particle.gravity_ = gravity;


	return particle;
}

///======================================================================
/// エミット
///======================================================================
void ParticleManager::Emit(
	std::string name, const Vector3& basePosition, const V3MinMax& positionDist,
	const FMinMax& scaledist, const V3MinMax& velocityDist, const Vector4& baseColor,
	const V4MinMax& colorDist, const float& lifeTime, const float& gravity,
	const Vector3& baseRotate, const Vector3& baseRotateSpeed, const V3MinMax& RotateDist,
	const V3MinMax& rotateSpeedDist, uint32_t count,const BlendMode& blendmode) {  // 新パラメータ追加

	// パーティクルグループが存在するか確認
	assert(particleGroups_.find(name) != particleGroups_.end() && "Error: Not Find ParticleGroup");

	// 指定されたパーティクルグループを取得
	ParticleGroup& particleGroup = particleGroups_[name];
	particleGroup.blendMode_ = blendmode;

	// 生成、グループ追加
	std::list<Particle> particles;
	for (uint32_t i = 0; i < count; ++i) {
		particles.emplace_back(MakeParticle(
			basePosition, positionDist, scaledist, velocityDist, baseColor,
			colorDist, lifeTime, gravity, baseRotate, baseRotateSpeed,
			RotateDist, rotateSpeedDist));
	}

	// グループに追加
	particleGroup.particles.splice(particleGroup.particles.end(), particles);
}


void ParticleManager::ResetAllParticles() {
	for (auto& groupPair : particleGroups_) {
		ParticleGroup& group = groupPair.second;

		// 全てのパーティクルを消去
		group.particles.clear();

		// インスタンシングデータをリセット
		for (uint32_t index = 0; index < group.instanceNum; ++index) {
			group.instancingData[index].WVP = MakeIdentity4x4();
			group.instancingData[index].World = MakeIdentity4x4();
			group.instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}
