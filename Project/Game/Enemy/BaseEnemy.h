#pragma once

#include "3d/ViewProjection.h"
// function
#include "Easing.h"
// class
#include "BaseObject/BaseObject.h"

class BaseEnemy : public BaseObject {
protected:
	bool isdeath_;
	
public:
	BaseEnemy();

	///========================================================================================
	///  public method
	///========================================================================================

	// 初期化、更新、描画
	virtual void Init(const Vector3& spownPos);
	virtual void Update();
	virtual void Draw(const ViewProjection& viewProjection);
	virtual void SpriteDraw(const ViewProjection& viewProjection);
	
	/// Hpバー処理
	virtual void DisplayHpBar(const ViewProjection& viewProjection);

	///========================================================================================
	///  getter method
	///========================================================================================
	bool GetIsDeath()const { return isdeath_; }

private:
	bool IsInView(const ViewProjection& viewProjection) const;
	
};
