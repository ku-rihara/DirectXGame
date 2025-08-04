#include "LockOn.h"
#include"input/Input.h"
#include "Matrix4x4.h"
#include "base/TextureManager.h"
//function
#include"MathFunction.h"
//class
#include "Enemy/BaseEnemy.h"
#include"Frame/Frame.h"

//初期化
void LockOn::Init() {
	//スプライトの読み込みと作成
	int TextureHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/anchorPoint.png");
	lockOnMark_.reset(Sprite::Create(TextureHandle, Vector2{ 640, 320 }, Vector4(1, 1, 1, 1)));
	lockOnMark_->SetAnchorPoint(Vector2(0.5f,0.5f));

	 kDegreeToRadian_ = 3.14f / 6.0f;
	// 最小距離
	 minDistance_ = 5.0f;
	// 最大距離
	 maxDistance_ = 90.0f;
	// 角度範囲
	 angleRange_ = 20.0f * kDegreeToRadian_;
}

void LockOn::Update(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection) {
			// ロックオンを外す
		if (target_) {
			if ((Input::IsTriggerPad(0, XINPUT_GAMEPAD_B))) {
				target_ = nullptr;
			}
			else if (IsOutOfRange(enemies, viewProjection)) {
				target_ = nullptr;
			}
		}
		else {
			if ((Input::IsTriggerPad(0, XINPUT_GAMEPAD_B))) { // ロックオンする
				// ロックオン対象の検索
				Search(enemies, viewProjection);
			}
		}

	// ロックオン継続
	if (target_) {
		// 敵のロックオン座標取得
		Vector3 positionWorld = target_->GetWorldPosition();
		// ワールド座標からスクリーン座標に変換
		Vector3 positionScreen = ScreenTransform(positionWorld, viewProjection);
		// Vector2に格納
		Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
		// 線形補間の計算
		LarpTimeIncrement(0.1f);
		lockOnMarkPos_ = Lerp(prePos_, positionScreenV2, lerpTime_);

		//// 距離に応じてスケールを調整
		//float distance = Distance(positionWorld,viewProjection.translation_);

		//// スケールを距離に反比例させる（距離が遠いほど小さくなる）
		//float scaleFactor = max(1.0f, distance / maxDistance_);

		//// スプライトの最大サイズを144に設定
		//const float maxSize = 144.0f;
		//float size = maxSize / scaleFactor; // 反比例させるために除算

		//// スプライトのサイズを設定
		//lockOnMark_->SetSize(Vector2(size, size));

		// スプライトの座標を設定
		lockOnMark_->SetPosition(lockOnMarkPos_);
		spriteRotation_ += Frame::DeltaTime();
		//回転を設定
		lockOnMark_->transform_.rotate.z=(spriteRotation_);
	}
}

void LockOn::Draw() {

	if (target_) {
		lockOnMark_->Draw();
	}
}

void LockOn::Search(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection) {
	Vector3 positionView = {}; //

	std::list<std::pair<float, const BaseEnemy*>> targets; // 目標

	// 全ての敵に対して順にロックオンを判定
	for (const std::unique_ptr<BaseEnemy>& enemy : enemies) {
		if (!enemy->GetIsDeath() && IsTargetRange(*enemy, viewProjection, positionView)) {
			targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
		}
	}

	// ロックオン対象をリセット
	target_ = nullptr;
	if (!targets.empty()) {
		// 距離で昇順にソート
		targets.sort([](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
		// ソートの結果一番近い敵をロックオン対象とする
		target_ = targets.front().second;
		// ロックオン前の座標を記録
		prePos_ = lockOnMark_->GetPosition();
		lerpTime_ = 0.0f; // 線形補間の進行度をリセット
	}
}

bool LockOn::IsOutOfRange(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection) {
	Vector3 positionView = {};
	// 全ての敵に対して順にロックオンを判定
	for (const std::unique_ptr<BaseEnemy>& enemy : enemies) {
		if (IsTargetRange(*enemy, viewProjection, positionView)) {
			// 範囲外ではない
			return false;
		}
	}
	return true;
}

bool LockOn::IsTargetRange(const BaseEnemy& enemy, const ViewProjection& viewProjection, Vector3& positionView) {

	// 敵のロックオン座標を取得
	Vector3 positionWorld = enemy.GetWorldPosition();
	// ワールド→ビュー座標系
	positionView = TransformMatrix(positionWorld, viewProjection.matView_);
	// 距離条件チェック
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
		// カメラ前方との角度を計算
		float actTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

		// 角度条件チェック（コーンに収まっているか）
		return (std::fabsf(actTangent) <= std::fabsf(angleRange_));
	}
	return false;
}
// 敵の中心座標取得をこのクラスでも作る
Vector3 LockOn::GetTargetPosition() const {
	if (ExistTarget()) {
		return target_->GetWorldPosition();
	}
	return Vector3();
}
void LockOn::OnEnemyDestroyed(BaseEnemy* enemy) {
	if (target_ == enemy) {
		target_ = nullptr;
	}
}
//線形補間タイムインクリメント
void LockOn::LarpTimeIncrement(float incrementTime) {
	lerpTime_ += incrementTime;
	if (lerpTime_ >= 1.0f) {
		lerpTime_ = 1.0f;
	}
}