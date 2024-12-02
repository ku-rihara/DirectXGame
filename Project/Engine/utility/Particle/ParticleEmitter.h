#pragma once

#include"MinMax.h"
#include"GrobalParamater/GlobalParameter.h"
///std
#include<string>

/// <summary>
/// Emitter
/// </summary>
class ParticleEmitter {
private:

	///=====================================================
	/// private variants
	///=====================================================
	std::string particleName_;       ///パーティクル名
	Vector3 basePos_;                ///基準座標
	V3MinMax positionDist_;          ///座標ランダム分配
	V3MinMax scaleDist_;             ///スケールランダム分配
	V3MinMax velocityDist_;          ///速度ランダム分配
	V4MinMax colorDist_;             ///色分配
	float lifeTime_;                 ///生存時間
	int32_t particleCount_;         ///パーティクル数

	GlobalParameter* globalParameter_;

public:

	// コンストラクタ 
	ParticleEmitter();

	///=====================================================
	/// public method
	///=====================================================

	/// 初期化
	static ParticleEmitter* CreateParticle(
		const std::string name, const std::string modelFilePath,
		const std::string& extension, const int32_t& maxnum);

	void AddGrobalParamater();
	void ApplyGlobalParameter();

	void Emit();///　エミット

	///=====================================================
    /// getter method
    ///=====================================================
	const std::string& GetParticleName()const { return particleName_; }


};