#include"ParticleManager.h"
#include"ParticleCommon.h"
#include"3d/ModelManager.h"
#include"base/TextureManager.h"
//frame
#include"Frame/Frame.h"
//Function
#include"random.h"
#include"Function/GetFile.h"
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
	SetAllParticleFile();
	
}

///============================================================
/// 更新
///============================================================
void ParticleManager::Update(const ViewProjection& viewProjection) {
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
			it->worldTransform_.rotation_.x += it->rotateSpeed_.x * Frame::DeltaTimeRate();
			it->worldTransform_.rotation_.y += it->rotateSpeed_.y * Frame::DeltaTimeRate();
			it->worldTransform_.rotation_.z += it->rotateSpeed_.z * Frame::DeltaTimeRate();

			///------------------------------------------------------------------------
			/// 重力の適用
			///------------------------------------------------------------------------
			it->velocity_.y += it->gravity_ * Frame::DeltaTime();

			///------------------------------------------------------------------------
			/// 変位更新
			///------------------------------------------------------------------------
			it->worldTransform_.translation_ += it->velocity_ * Frame::DeltaTime();

			///------------------------------------------------------------------------
			/// ビルボードまたは通常の行列更新
			///------------------------------------------------------------------------

			if (group.parm.isBillBord) {

				it->worldTransform_.BillboardUpdateMatrix(viewProjection, group.parm.billBordType, group.parm.adaptRotate_);
			} else {
				it->worldTransform_.UpdateMatrix();
			}

			// 時間を進める
			it->currentTime_ += Frame::DeltaTime();
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

			instancingData[instanceIndex].World = it->worldTransform_.matWorld_;

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
			ParticleCommon::GetInstance()->PreDraw(commandList, group.parm.blendMode);
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
ParticleManager::Particle ParticleManager::MakeParticle(const ParticleEmitter::Parameters& paramaters) {

	Particle particle;

	particle.lifeTime_ = paramaters.lifeTime;
	particle.currentTime_ = 0.0f;
	particle.worldTransform_.Init();

	///------------------------------------------------------------------------
	/// 座標
	///------------------------------------------------------------------------
	Vector3 randomTranslate = {
		Random::Range(paramaters.positionDist.min.x, paramaters.positionDist.max.x),
		Random::Range(paramaters.positionDist.min.y, paramaters.positionDist.max.y),
		Random::Range(paramaters.positionDist.min.z, paramaters.positionDist.max.z)
	};
	particle.worldTransform_.translation_ = paramaters.targetPos + paramaters.emitPos + randomTranslate;

	///------------------------------------------------------------------------
	/// 速度
	///------------------------------------------------------------------------
	particle.velocity_ = {
		Random::Range(paramaters.velocityDist.min.x,paramaters.velocityDist.max.x),
		Random::Range(paramaters.velocityDist.min.y,paramaters.velocityDist.max.y),
		Random::Range(paramaters.velocityDist.min.z,paramaters.velocityDist.max.z)
	};

	///------------------------------------------------------------------------
	/// 回転
	///------------------------------------------------------------------------
	if (paramaters.isRotateforDirection) {
		// 進行方向（速度）を基に回転を計算

		particle.worldTransform_.rotation_ = DirectionToEulerAngles(particle.velocity_,*viewProjection_);
	} else {
		// ランダム回転を設定
		Vector3 rotate = {
			Random::Range(paramaters.rotateDist.min.x, paramaters.rotateDist.max.x),
			Random::Range(paramaters.rotateDist.min.y, paramaters.rotateDist.max.y),
			Random::Range(paramaters.rotateDist.min.z, paramaters.rotateDist.max.z)
		};


		particle.worldTransform_.rotation_ = (paramaters.baseRotate) + rotate;
	}

	///------------------------------------------------------------------------
	/// 回転スピード
	///------------------------------------------------------------------------
	Vector3 rotateSpeed = {
		Random::Range(paramaters.rotateSpeedDist.min.x, paramaters.rotateSpeedDist.max.x),
		Random::Range(paramaters.rotateSpeedDist.min.y, paramaters.rotateSpeedDist.max.y),
		Random::Range(paramaters.rotateSpeedDist.min.z, paramaters.rotateSpeedDist.max.z)
	};

	particle.rotateSpeed_ = rotateSpeed;

	///------------------------------------------------------------------------
	/// スケール
	///------------------------------------------------------------------------
	if (paramaters.isScalerScale) {// スカラー
		float scale = Random::Range(paramaters.scaleDist.min, paramaters.scaleDist.max);
		particle.worldTransform_.scale_ = { scale, scale, scale };
	} else {/// V3
		Vector3 ScaleV3 = {
			Random::Range(paramaters.scaleDistV3.min.x, paramaters.scaleDistV3.max.x),
			Random::Range(paramaters.scaleDistV3.min.y, paramaters.scaleDistV3.max.y),
			Random::Range(paramaters.scaleDistV3.min.z, paramaters.scaleDistV3.max.z)
		};

		particle.worldTransform_.scale_ = ScaleV3;
	}
	///------------------------------------------------------------------------
	/// 色
	///------------------------------------------------------------------------
	Vector4 randomColor = {
		Random::Range(paramaters.colorDist.min.x, paramaters.colorDist.max.x),
		Random::Range(paramaters.colorDist.min.y, paramaters.colorDist.max.y),
		Random::Range(paramaters.colorDist.min.z, paramaters.colorDist.max.z),
		0.0f
	};

	particle.color_ = paramaters.baseColor + randomColor;

	///------------------------------------------------------------------------
	/// 重力
	///------------------------------------------------------------------------
	particle.gravity_ = paramaters.gravity;

	return particle;
}


///======================================================================
/// エミット
///======================================================================
void ParticleManager::Emit(
	std::string name, const ParticleEmitter::Parameters&
	paramaters, const ParticleEmitter::GroupParamaters& groupParamaters, const int32_t& count) {  // 新パラメータ追加

	// パーティクルグループが存在するか確認
	assert(particleGroups_.find(name) != particleGroups_.end() && "Error: Not Find ParticleGroup");

	// 指定されたパーティクルグループを取得
	ParticleGroup& particleGroup = particleGroups_[name];
	particleGroup.parm.blendMode = groupParamaters.blendMode;
	particleGroup.parm.isBillBord = groupParamaters.isBillBord;
	particleGroup.parm.billBordType = groupParamaters.billBordType;
	particleGroup.parm.adaptRotate_ = groupParamaters.adaptRotate_;

	// 生成、グループ追加
	std::list<Particle> particles;
	for (uint32_t i = 0; i < uint32_t(count); ++i) {
		particles.emplace_back(MakeParticle(paramaters));
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


Vector3 ParticleManager::DirectionToEulerAngles(const Vector3& direction, const ViewProjection& view) {

	// ベクトル正規化
	Vector3 rdirection = direction.Normalize();

	// カメラの回転を反映した回転行列を作成
	Matrix4x4 rotateCameraMatrix = MakeRotateMatrix(Vector3(-view.rotation_.x, -view.rotation_.y, -view.rotation_.z));
	rdirection = TransformNormal(rdirection, rotateCameraMatrix);

	// 基準ベクトル(上方向)をカメラの回転で変換
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	up = TransformNormal(up, rotateCameraMatrix);

	// 方向変換行列を作成
	Matrix4x4 dToDMatrix = DirectionToDirection(up, rdirection);

	// 方向変換行列からオイラー角を抽出
	Vector3 angle = ExtractEulerAngles(dToDMatrix);

	return  angle;
}

void ParticleManager::SetViewProjection(const ViewProjection* view){

	viewProjection_ = view;
}

void ParticleManager::SetAllParticleFile() {
	particleFiles_ = GetFileNamesForDyrectry(dyrectry_);
}