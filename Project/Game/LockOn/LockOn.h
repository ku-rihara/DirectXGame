#pragma once
#include "2d/Sprite.h"
#include "3d/ViewProjection.h"

#include "Vector3.h"

#include <list>
#include <memory>

class BaseEnemy;
class LockOn {
private:

	// ロックオンマーク用スプライト
	std::unique_ptr<Sprite> lockOnMark_;
	// ロックオン対象
	const BaseEnemy* target_ = nullptr;
	/*bool isRockOn_ = false;*/

	float kDegreeToRadian = 3.14f / 6.0f;
	// 最小距離
	float minDistance_ = 10.0f;
	// 最大距離
	float maxDistance_ = 120.0f;
	// 角度範囲
	float angleRange_ = 20.0f * kDegreeToRadian;
	//線形補間タイム
	float lerpTime_;
	//ロックオン前の座標
	Vector2 prePos_;
	//LockOnMarkに格納する変数
	Vector2 lockOnMarkPos_;
	float spriteRotation_;

public:
	void Init();
	void Update(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);
	void Draw();

	//線形補間タイムインクリメント
	void LarpTimeIncrement(float incrementTime);

	bool IsTargetRange(const BaseEnemy& enemy, const ViewProjection& viewProjection, Vector3& positionView);
	bool IsOutOfRange(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);
	void Search(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);
	Vector3 GetTargetPosition() const;
	bool ExistTarget() const { return target_ ? true : false; }
	void OnEnemyDestroyed(BaseEnemy* enemy);
	//Getter
	const BaseEnemy* GetEnemyTarget() { return target_; }
	
};
