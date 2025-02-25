#pragma once

#include"MinMax.h"
#include"3d/ViewProjection.h"
/// editor
#include"utility/Editor/GlobalParameter.h"

#include"utility/Editor/RailManager.h"
///std
#include<string>
#include<vector>

/// <summary>
/// Emitter
/// </summary>
enum class WorldTransform::BillboardType;
struct WorldTransform::AdaptRotate;
enum class BlendMode;
class ParticleEmitter {
public:
	struct GroupParamaters {
		BlendMode blendMode;
		bool isBillBord;
		bool isShot;
		WorldTransform::BillboardType  billBordType;
		WorldTransform::AdaptRotate adaptRotate_;
	};

	// パーティクル設定を統合する構造体
	struct Parameters {
		Vector3 targetPos;                    // 対象座標
		Vector3 emitPos;                      // 発生座標
		V3MinMax positionDist;                // 座標ランダム分配
		bool isScalerScale;                   //スカラーのスケールにするか
		FMinMax scaleDist;                    // スケールランダム分配
		V3MinMax scaleDistV3;                 // スケールランダム分配
		V3MinMax velocityDist;                // 速度ランダム分配
		Vector4 baseColor;                    // 基準の色
		V4MinMax colorDist;                   // 色ランダム分配
		float lifeTime;                       // 生存時間
		float gravity;                        // 重力パラメータ
		Vector3 baseRotate;                   // 回転基準
		V3MinMax rotateDist;                  // 回転ランダム分配
		V3MinMax rotateSpeedDist;             // 回転スピード分配
		bool isRotateforDirection;            // 方向回転フラグ
	};
private:

	///=====================================================
	/// private variants
	///=====================================================
	std::string particleName_;                            // パーティクル名
	int32_t particleCount;

	/// パーティクル設定
	Parameters parameters_;
	GroupParamaters groupParamaters_;

	int preBillBordType_;
	bool preIsShot_;
	float currentTime_;                                   ///現在の時間
	float intervalTime_;                                  ///発生するまでの間隔

	/// 発生位置関連
	std::unique_ptr<Object3d> obj3d_;                     // 発生位置のオブジェクト
	WorldTransform emitBoxTransform_;                    // 発生位置のワールド変換

	/// レール関連
	bool isMoveForRail_;                                 // レールに沿って動くか
	float moveSpeed_;                                    // 移動速度
	std::unique_ptr<RailManager> railManager_;           // レールマネージャ

	/// その他
	const std::string folderName_ = "Particle";
	std::string editorMessage_;                          // エディタ用メッセージ
	GlobalParameter* globalParameter_;                   // グローバルパラメータ
	

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
	void UpdateEmitTransform();

	void RailDraw(const ViewProjection& viewProjection);
	void DebugDraw(const ViewProjection& viewProjection);

	///=====================================================
	/// getter method
	///=====================================================
	const std::string& GetParticleName()const { return particleName_; }
	

	///=====================================================
	/// setter method
	///=====================================================
	void SetParentBasePos(WorldTransform* parent);
	void SetTextureHandle(const uint32_t& hanle);
	void SetTargetPosition(const Vector3& pos) { parameters_.targetPos = pos; }

	//imgui化すべき
	void SetIsRotateForDirection(const bool& is) { parameters_.isRotateforDirection = is; }
	void SetBlendMode(const BlendMode& blendmode);
	void SetBillBordType(const WorldTransform::BillboardType& billboardType);
	///=====================================================
	/// Editor 
	///=====================================================

	///update
	void Update();
	void EditorUpdate();

	/// globalParamater
	void AddParmGroup();
	void SetValues();

	void ApplyGlobalParameter(const std::string&particleName);
	void ParmLoadForImGui();
	void ParmSaveForImGui();

	void ParticleChange();

	std::vector<std::string> GetParticleFiles(const std::string& directory);
};
