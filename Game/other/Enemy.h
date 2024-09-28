#pragma once
#include <memory>
#include <vector>
#include "Model.h"
#include "BaseCharacter.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
class Enemy:public BaseCharacter {
private:
	enum Parts {
		IndexHead,
		IndexLeftThorn,
		IndexRightThorn,
		
	};
	const int partsnum = 3;
	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	// 3Dモデル
	std::vector<Model*> modelParts_;
	//速度
	Vector3 velocity_;
	//シリアルナンバー
	uint32_t serialNum_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNum_;
	// 浮遊ギミックの媒介変数
	float animeParamater_ = 0.0f;

	
public:
	Enemy();

	void Init(const std::vector<Model*>& models) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	Vector3 GetBaseCenterPosition()const  override;
	void AnimationInit()override;
	void AnimationUpdate()override;
	

	 Vector3 GetCenterPos()const;
	 //getter
	 uint32_t GetSerialNum() const { return serialNum_; }
	// setter
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	
};


