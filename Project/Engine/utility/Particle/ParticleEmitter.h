#pragma once

#include"MinMax.h"
#include"utility/Editor/GlobalParameter.h"
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
	Vector4 baseColor_;              ///基準の色
	float lifeTime_;                 ///生存時間
	int32_t particleCount_;         ///パーティクル数

	/// name
	const std::string dyrectryPath = "./Resources/ParticleParamater/";
	std::string editorMessage_;

	///editor
	GlobalParameter* globalParameter_;

public:

	// コンストラクタ 
	ParticleEmitter();

	///=====================================================
	/// public method
	///=====================================================

	/// 初期化
	static ParticleEmitter* CreateParticle(
		const std::string& name, const std::string& modelFilePath,
		const std::string& extension, const int32_t& maxnum);

	void Init();/// 初期化
	void Emit();///　エミット

	///=====================================================
	/// getter method
	///=====================================================
	const std::string& GetParticleName()const { return particleName_; }

	///=====================================================
	/// Editor 
	///=====================================================
	void AddParmGroup();
	/// パラメータをImGuiで編集する
	void EditorUpdate();
};
