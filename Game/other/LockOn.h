#pragma once
#include "Sprite.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include <list>
#include <memory>

class Enemy;
class LockOn {
private:

	// ロックオンマーク用スプライト
	std::unique_ptr<Sprite> lockOnMark_;
	// ロックオン対象
	const Enemy* target_ = nullptr;
	/*bool isRockOn_ = false;*/

	float kDegreeToRadian = 3.14f / 6.0f;
	// 最小距離
	float minDistance_ = 10.0f;
	// 最大距離
	float maxDistance_ = 40.0f;
	// 角度範囲
	float angleRange_ = 20.0f * kDegreeToRadian;

public:
	void Init();
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);
	void Draw();

	bool IsTargetRange(const Enemy& enemy, const ViewProjection& viewProjection, Vector3& positionView);
	bool IsOutOfRange(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);
	void Search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);
	Vector3 GetTargetPosition() const;
	bool ExistTarget() const { return target_ ? true : false; }

	//Getter
	const Enemy* GetEnemyTarget() { return target_; }
	/*bool GetIsRockOn() { return isRockOn_; }*/
};
