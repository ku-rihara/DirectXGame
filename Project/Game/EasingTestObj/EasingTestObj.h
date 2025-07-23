#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
#include"Easing.h"
#include"EasingSequence.h"
class EasingTestObj {
private:

    enum class AdaptType {
        Position, // 位置
        Rotation, // 回転
        Scale, // 拡大縮小
	};

private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
	std::unique_ptr<Object3d> object3D_=nullptr;

	Easing<Vector3> easing_;

	EasingSequence<Vector3> easingSequence_;
	
public:
    EasingTestObj();
    ~EasingTestObj();
	void Init();
	void Update();
	void Draw(ViewProjection& viewProjection);

	void Debug();
	void ScaleEasing();

	 Easing<Vector3>& GetEasingData()  { return easing_; }
};
