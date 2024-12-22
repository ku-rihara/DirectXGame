#pragma once

#include"MinMax.h"
#include"3d/ViewProjection.h"
/// editor
#include"utility/Editor/GlobalParameter.h"
#include"utility/Editor/EmitControlPosManager.h"

#include"utility/Particle/EmitRailManager.h"
///std
#include<string>
#include<vector>

/// <summary>
/// Emitter
/// </summary>
class ParticleEmitter {
private:

	///=====================================================
	/// private variants
	///=====================================================
	
	/// obj
	std::unique_ptr<Object3d>obj3d_;             /// 発生位置のオブジェ
	WorldTransform emitBoxTransform_;

	/// name								      
	std::string particleName_;                     ///パーティクル名
	std::string railFilePath_;                     // レールデータの名前
									               
	/// time						               
	float currentTime_;                            ///現在の時間
	float intervalTime_;                           ///発生するまでの間隔

	/// target
	Vector3 targetPos_;                            /// 対象座標
											      
	/// base								      
	Vector3 emitPos_;                              ///発生座標
	Vector3 basePos_;                              ///基準座標
	Vector3 baseRotate_;                           ///回転基準
	Vector3 baseRotateSpeed_;                      /// 回転スピード基準
	Vector4 baseColor_;                           ///基準の色
								                  
	/// rondom					                  
	V3MinMax positionDist_;                       ///座標ランダム分配
	FMinMax scaleDist_;                           ///スケールランダム分配
	V3MinMax rotateSpeedDist_;                    /// 回転スピード分配
	V3MinMax rotateDist_;                         /// 回転ランダム分配
	V3MinMax velocityDist_;                       ///速度ランダム分配
	V4MinMax colorDist_;                          ///色分配
								                  
	/// etc						                  
	float lifeTime_;                              ///生存時間
	float gravity_;                               ///重力パラメータ
	int32_t particleCount_;                       ///パーティクル数
											      
	/// Line								      
	bool isMoveForRail_;                           ///レールに沿って動くか
	float moveSpeed_;                              /// 移動速さ
	std::unique_ptr<EmitRailManager> railManager_; /// レールマネージャ
	std::unique_ptr
	<EmitControlPosManager>emitControlPosManager_;

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
		const std::string& extension, const int32_t& maxnum, const bool& isFirst);

	void Init(const bool& isFirst);/// 初期化
	void Emit();///　エミット
	void UpdateEmitTransform();

	void RailDraw(const ViewProjection&viewProjection);
	void DebugDraw(const ViewProjection& viewProjection);

	///=====================================================
	/// getter method
	///=====================================================
	const std::string& GetParticleName()const { return particleName_; }


	///=====================================================
    /// setter method
    ///=====================================================
	void SetParentBasePos(WorldTransform*parent);
	void SetTextureHandle(const uint32_t& hanle);
	void SetTargetPosition(const Vector3& pos) { targetPos_ = pos; }
	///=====================================================
	/// Editor 
	///=====================================================

	///update
	void Update();

	/// globalParamater
	void AddParmGroup();
	void SetValues();

	void ApplyGlobalParameter();
	void ParmLoadForImGui();

	
};
