#include "PlayerAttackRendition.h"
#include "AttackEffect/AttackEffect.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"

void PlayerAttackRendition::Init(Player* player, PlayerComboAttackData* playerComboAttackData) {
    pPlayer_               = player;
    playerComboAttackData_ = playerComboAttackData;
    Reset();
}

void PlayerAttackRendition::Reset() {
    currentTime_ = 0.0f;
    isPlayed_.fill(false);
    isObjAnimPlayed_.fill(false);
}

void PlayerAttackRendition::Update(float deltaTime) {
    if (!pPlayer_ || !playerComboAttackData_) {
        return;
    }

    currentTime_ += deltaTime;

    auto& renditionData = playerComboAttackData_->GetRenditionData();

    // 通常演出の更新
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamFromIndex(i);

        // トリガー条件がヒットの場合のcontinue処理
        if (param.triggerByHit && !pPlayer_->GetPlayerCollisionInfo()->GetIsHit()) {
            continue;
        }

        // すでに再生済みならスキップ
        if (isPlayed_[i]) {
            continue;
        }

        // startTiming に達したら発動
        if (currentTime_ >= param.startTiming && param.fileName != "") {
            switch (static_cast<PlayerAttackRenditionData::Type>(i)) {
            case PlayerAttackRenditionData::Type::CameraAction:
                pPlayer_->GetGameCamera()->PlayAnimation(param.fileName);
                break;

            case PlayerAttackRenditionData::Type::HitStop:
                pPlayer_->GetAttackEffect()->PlayHitStop(param.fileName);
                break;

            case PlayerAttackRenditionData::Type::ShakeAction:
                pPlayer_->GetGameCamera()->PlayShake(param.fileName);
                break;

            case PlayerAttackRenditionData::Type::PostEffect:
                pPlayer_->GetAttackEffect()->PlayPostEffect(param.fileName);
                break;

            default:
                break;
            }

            // 再生済みに設定
            isPlayed_[i] = true;
        }
    }

    // オブジェクトアニメーションの更新
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        // すでに再生済みならスキップ
        if (isObjAnimPlayed_[i]) {
            continue;
        }

        // ファイル名が空ならスキップ
        if (param.fileName.empty() || param.fileName == "None") {
            isObjAnimPlayed_[i] = true;
            continue;
        }

        // startTiming に達したら発動
        if (currentTime_ >= param.startTiming) {
            switch (static_cast<PlayerAttackRenditionData::ObjAnimationType>(i)) {
            case PlayerAttackRenditionData::ObjAnimationType::Head:
                pPlayer_->GetObject3D()->transform_.PlayObjEaseAnimation("Player", param.fileName);
                break;

            case PlayerAttackRenditionData::ObjAnimationType::RightHand:
                pPlayer_->GetRightHand()->GetObject3D()->transform_.PlayObjEaseAnimation("RightHand", param.fileName);
                break;

            case PlayerAttackRenditionData::ObjAnimationType::LeftHand:
                pPlayer_->GetLeftHand()->GetObject3D()->transform_.PlayObjEaseAnimation("LeftHand", param.fileName);
                break;

            case PlayerAttackRenditionData::ObjAnimationType::MainHead:
                pPlayer_->MainHeadAnimationStart(param.fileName);
                break;

            default:
                break;
            }

            // 再生済みに設定
            isObjAnimPlayed_[i] = true;
        }
    }
}