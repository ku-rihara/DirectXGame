#include "LockOn.h"
#include"input/Input.h"
#include "Matrix4x4.h"
#include "base/TextureManager.h"
//function
#include"MathFunction.h"
//class
#include "Enemy/BaseEnemy.h"
#include"Player/Player.h"
#include"Frame/Frame.h"

//初期化
void LockOn::Init() {
	
	int TextureHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/anchorPoint.png");
    lockOnMark_.reset(Sprite::Create(TextureHandle, Vector2::ZeroVector(), Vector4::kWHITE()));
	lockOnMark_->SetAnchorPoint(Vector2(0.5f,0.5f));

	
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
		LerpTimeIncrement(0.1f);
		lockOnMarkPos_ = Lerp(prePos_, positionScreenV2, lerpTime_);


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

bool LockOn::IsTargetRange(const LockOnVariant& target, const Player* player, Vector3& relativePosition) const {
    // プレイヤーの位置と向きを取得
    Vector3 playerPos     = player->GetWorldPosition();
    Vector3 playerForward = player->GetForwardVector();

    // ターゲットの位置を取得
    Vector3 targetPos = GetPosition(target);

    // プレイヤーからターゲットへの相対ベクトル
    Vector3 toTarget = targetPos - playerPos;
    relativePosition = toTarget;

   
    // 距離チェック
    float distance = toTarget.Length();
    if (distance < minDistance_ || distance > maxDistance_) {
        return false;
    }

    // プレイヤーの前方ベクトルとの角度チェック
    if (distance > 0.001f) { // ゼロ除算回避
        Vector3 toTargetNormalized = toTarget.Normalize();
        float dot                  = Vector3::Dot(playerForward, toTargetNormalized);

        // dotを-1〜1の範囲にクランプ
        dot         = std::clamp(dot, -1.0f, 1.0f);
        float angle = std::acos(dot);

        // 角度が範囲内かチェック（angleRange_は度単位）
        return angle <= ToRadian(angleRange_);
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
void LockOn::LerpTimeIncrement(float incrementTime) {
	lerpTime_ += incrementTime;
	if (lerpTime_ >= 1.0f) {
		lerpTime_ = 1.0f;
	}
}