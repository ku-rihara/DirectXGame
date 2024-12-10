#pragma once

#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "vector"
#include <memory>
#include <optional>
#include<random>
// function
#include "Easing.h"
// class

#include "BaseObject/BaseObject.h"

class BaseEnemy : public BaseObject {
protected:
	
	
public:
	BaseEnemy();
	// 初期化、更新、描画
	virtual void Init(const Vector3& spownPos);
	virtual void Update(const ViewProjection& viewProjection);
	virtual void Draw(const ViewProjection& viewProjection);
	virtual void SpriteDraw(const ViewProjection& viewProjection);
	
	
private:
	bool IsInView(const ViewProjection& viewProjection) const;
	
};
