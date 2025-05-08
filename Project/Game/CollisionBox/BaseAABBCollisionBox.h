#pragma once

#include"Collider/AABBCollider.h"
#include"3d/WorldTransform.h"
#include"utility/ParameterEditor/GlobalParameter.h"
#include<string>

class BaseAABBCollisionBox :public AABBCollider {
public:

    /// ====================================================
    ///  public method
    /// ====================================================

    virtual void Init();
    virtual void Update();
    virtual void Draw();

    virtual void IsAdapt(bool is);
   

    // Collision発生時のコールバック関数
    virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
    virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

protected:
	/// *TODO 
	/// Imguiで編集できるエディターを作成

	GlobalParameter* globalParamater_;

	//transform,offset
	WorldTransform transform_;
	Vector3 offset_;

    //生存時間
	float aliveTime_;
public:
	std::string tag_;

public:
	/// getter
	virtual Vector3 GetCollisionPos() const override;
	std::string GetTag()const { return tag_; }

    /// setter
	virtual void SetSize(const Vector3& size);
    virtual void SetPosition(const Vector3& position);
    virtual void SetOffset(const Vector3& offset);
};