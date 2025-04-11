#pragma once

#include"Collider/AABBCollider.h"
#include"3d/WorldTransform.h"
#include"utility/ParameterEditor/GlobalParameter.h"
#include<string>

class BaseAABBCollisionBox :public AABBCollider {
protected:
	/// *TODO 
	/// Imguiで編集できるエディターを作成

	GlobalParameter* globalParamater_;
	WorldTransform transform_;
	float scalerOffset_;
	Vector3 offset_;
	float aliveTime_;
public:
	std::string tag_;
public:

	virtual void Init();
	virtual void Update();
	virtual void Draw();


	virtual void IsAdapt(bool is);
	virtual void SetSize(const Vector3& size);
	virtual void SetPosition(const Vector3&position);
	virtual void SetOffset(const Vector3& offset);
	
	// 当たり
	virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;
	
	virtual Vector3 GetCollisionPos() const override;

	std::string GetTag()const { return tag_; }
};