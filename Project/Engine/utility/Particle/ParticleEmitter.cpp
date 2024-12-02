#include"ParticleEmitter.h"
#include"ParticleManager.h"


///=====================================================
/// コンストラクタ
///=====================================================
ParticleEmitter::ParticleEmitter() {};

ParticleEmitter* ParticleEmitter::CreateParticle(
	const std::string name, const std::string modelFilePath,
	const std::string& extension, const int32_t& maxnum) {

	std::unique_ptr<ParticleEmitter>emitter = std::make_unique<ParticleEmitter>();

	/// グループ作成
	emitter->particleName_ = name;
	ParticleManager::GetInstance()->CreateParticleGroup(
		emitter->particleName_, modelFilePath, extension, maxnum);

	///GrobalParamater
	emitter->AddGrobalParamater();

	return emitter.release();
}

///=====================================================
///  GrobalParamater
///===================================================== 
void ParticleEmitter::AddGrobalParamater() {

	globalParameter_ = GlobalParameter::GetInstance();
	// パラメータグループを追加
	globalParameter_->CreateGroup(particleName_);
	globalParameter_->AddItem(particleName_, "basePos", basePos_);
	globalParameter_->AddItem(particleName_, "PositionMin", positionDist_.min);
	globalParameter_->AddItem(particleName_, "PositionMax", positionDist_.max);
	globalParameter_->AddItem(particleName_, "scaleMin", scaleDist_.min);
	globalParameter_->AddItem(particleName_, "scaleMax", scaleDist_.max);
	globalParameter_->AddItem(particleName_, "velocityMin", velocityDist_.min);
	globalParameter_->AddItem(particleName_, "velocityMax", velocityDist_.max);
	globalParameter_->AddItem(particleName_, "colorMin", colorDist_.min);
	globalParameter_->AddItem(particleName_, "colorMax", colorDist_.max);
	globalParameter_->AddItem(particleName_, "lifeTime", lifeTime_);
	globalParameter_->AddItem(particleName_, "Count", particleCount_);
}

///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void ParticleEmitter::ApplyGlobalParameter() {

	basePos_ = globalParameter_->GetValue<Vector3>(particleName_, "basePos");
	positionDist_.min=globalParameter_->GetValue<Vector3>(particleName_, "PositionMin");
	positionDist_.max=globalParameter_->GetValue<Vector3>(particleName_, "PositionMax");
	scaleDist_.min=globalParameter_->GetValue<Vector3>(particleName_, "scaleMin");
	scaleDist_.max=globalParameter_->GetValue<Vector3>(particleName_, "scaleMax");
	velocityDist_.min=globalParameter_->GetValue<Vector3>(particleName_, "velocityMin");
	velocityDist_.max=globalParameter_->GetValue<Vector3>(particleName_, "velocityMax");
	colorDist_.min=globalParameter_->GetValue<Vector4>(particleName_, "colorMin");
	colorDist_.max=globalParameter_->GetValue<Vector4>(particleName_, "colorMax");
	lifeTime_=globalParameter_->GetValue<float>(particleName_, "lifeTime");
	particleCount_=globalParameter_->GetValue<int32_t>(particleName_, "Count");
}


///=====================================================
/// 値セット
///=====================================================
//void ParticleEmitter::SetVariant(
//	const Vector3& basePos,                ///基準座標
//	const V3MinMax& positionDist,          ///座標ランダム分配
//	const V3MinMax& scaleDist,             ///スケールランダム分配
//	const V3MinMax& velocityDist,          ///速度ランダム分配
//	const V4MinMax& colorDist,             ///色分配
//	const float& lifeTime,                 ///生存時間
//	const int32_t& particleCount) {       ///パーティクル数
//
//	basePos_ = basePos;
//	positionDist_ = positionDist;
//	scaleDist_ = scaleDist;
//	velocityDist_ = velocityDist;
//	colorDist_ = colorDist;
//	lifeTime_ = lifeTime;
//	particleCount_ = particleCount;
//
//}

///=====================================================
/// エミット
///=====================================================
void ParticleEmitter::Emit() {

	ParticleManager::GetInstance()->Emit(
		particleName_, basePos_, positionDist_, scaleDist_,
		velocityDist_, colorDist_, lifeTime_, particleCount_);
}