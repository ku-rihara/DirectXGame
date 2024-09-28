#include "LockOn.h"
#include "Enemy.h"
#include "JoyState.h"
#include "Matrix4x4.h"
#include "TextureManager.h"
#include"Input.h"

void LockOn::Init() {
	/*uint32_t TextureHandle = TextureManager::Load("./Resources/anchorPoint.png");

	lockOnMark_.reset(Sprite::Create(TextureHandle, Vector2{640, 320}, Vector4(1, 1, 1, 1), Vector2(0.5f, 0.5f)));*/
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	if (Input::GetInstance()->GetJoystickState(0, joyState) && Input::GetInstance()->GetJoystickStatePrevious(0, joyStatePre)) {
		// ロックオン外す
		if (target_) {
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B && joyStatePre.Gamepad.wButtons != XINPUT_GAMEPAD_B) {
				target_ = nullptr;

			}
			else if (IsOutOfRange(enemies, viewProjection)) {
				target_ = nullptr;
			}
		}
		else {
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B && joyStatePre.Gamepad.wButtons != XINPUT_GAMEPAD_B) { // ロックオンする
				// ロックオン対象の検索
				Search(enemies, viewProjection);
			}
		}
	}

	// ロックオン継続
	if (target_) {
		// 敵のロックオン座標取得
		Vector3 positionWorld = target_->GetCenterPos();
		// ワールド座標からスクリーン座標に変換
		Vector3 positionScreen = ScreenTransform(positionWorld, viewProjection);
		// Vector2に格納
		Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
		// スプライトの座標を設定
		/*lockOnMark_->SetPosition(positionScreenV2);*/
	}
}

void LockOn::Draw() {

	/*if (target_) {
		lockOnMark_->Draw();
	}*/
}

void LockOn::Search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	Vector3 positionView = {}; //

	std::list<std::pair<float, const Enemy*>> targets; // 目標

	// 全ての敵に対して順にロックオンを判定
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		if (IsTargetRange(*enemy, viewProjection, positionView)) {
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
	}
}

bool LockOn::IsOutOfRange(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	Vector3 positionView = {};
	// 全ての敵に対して順にロックオンを判定
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		if (IsTargetRange(*enemy, viewProjection, positionView)) {
			// 範囲外ではない
			return false;
		}
	}
	return true;
}

bool LockOn::IsTargetRange(const Enemy& enemy, const ViewProjection& viewProjection, Vector3& positionView) {

	// 敵のロックオン座標を取得
	Vector3 positionWorld = enemy.GetCenterPos();
	// ワールド→ビュー座標系
	positionView = Transform(positionWorld, viewProjection.matView_);
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
		return target_->GetCenterPos();
	}
	return Vector3();
}