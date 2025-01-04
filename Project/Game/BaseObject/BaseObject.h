#pragma once
#include"3d/ViewProjection.h"
#include"3d/WorldTransform.h"
#include"3d/Object3d.h"

//std
#include<string>

class BaseObject {
protected:

	/// ===================================================
	///protected variaus
	/// ===================================================

	/*uint32_t partsnum_;*/

	// モデル配列データ
     std::unique_ptr<Object3d> obj3d_;
	// ベースのワールド変換データ
	WorldTransform transform_;

	//カラー
	/*ObjColor objColor_;*/

	/*AABB aabb_;*/

public:
	virtual ~BaseObject() = default;
	/// ===================================================
	///public method
	/// ===================================================

	//初期化、更新、描画
	virtual void Init();
	virtual void Update();
	virtual void Draw(const ViewProjection& viewProjection);
	
	virtual void CreateModel(
		const std::string& modelname, const std::string& name);

	// 中心座標取得
	virtual Vector3 GetWorldPosition() const;
	

	/// ===================================================
    ///getter 
    /// ===================================================
	const WorldTransform& GetTransform() { return transform_; }
	/// ===================================================
    ///setter 
    /// ===================================================
	void SetParent(WorldTransform* parent);
	void SetColor(const Vector4& color);
	void SetWorldPosition(Vector3 pos) { transform_.translation_ = pos; }
	void SetWorldPositionY(float pos) { transform_.translation_.y = pos; }
	void SetWorldPositionX(float pos) { transform_.translation_.x = pos; }
	void SetWorldPositionZ(float pos) { transform_.translation_.z = pos; }
	void AddPosition(Vector3 pos) { transform_.translation_ += pos; }
	void SetRotation(Vector3 rotate) { transform_.rotation_ = rotate; }
	void SetRotationY(float rotate) { transform_.rotation_.y = rotate; }
	void SetScale(Vector3 scale) { transform_.scale_ = scale; }
};
