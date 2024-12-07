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

	/// name
	std::string particleName_;       ///パーティクル名

	/// time
	float currentTime_;
	float intervalTime_;

	/// base
	Vector3 basePos_;                ///基準座標
	Vector3 baseRotate_;
	Vector3 baseRotateSpeed_;       /// 回転スピード基準
	Vector4 baseColor_;              ///基準の色

	/// rondom
	V3MinMax positionDist_;          ///座標ランダム分配
	FMinMax scaleDist_;             ///スケールランダム分配
	V3MinMax rotateSpeedDist_;     /// 回転スピード分配
	V3MinMax rotateDist_;           /// 回転ランダム分配
	V3MinMax velocityDist_;          ///速度ランダム分配
	V4MinMax colorDist_;             ///色分配

	/// etc
	float lifeTime_;                 ///生存時間
	float gravity_;                 ///重力パラメータ
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
	void ApplyGlobalParameter();
	void ParmLoadForImGui();

	void ImGuiUpdate();
};
