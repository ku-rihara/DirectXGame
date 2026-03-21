#include "PlayerAttackRendition.h"
#include "Player/Components/Effect/AttackEffect/AttackEffect.h"
#include "Audio/Audio.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "Input/Input.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"

void PlayerAttackRendition::Init(Player* player, PlayerComboAttackData* playerComboAttackData) {
    pPlayer_               = player;
    playerComboAttackData_ = playerComboAttackData;
    Reset();
}

void PlayerAttackRendition::Reset() {
    currentTime_  = 0.0f;
    isRendition_  = false;
    isPlayed_.fill(false);
    isPlayedOnHit_.fill(false);
    isObjAnimePlayed_.fill(false);
    hasTriggeredHitEffects_ = false;
    previousHasHit_         = false;

    // ポストエフェクトリスト再生フラグをリセット（サイズは Update で設定）
    isPostEffectPlayed_.clear();
    isPostEffectOnHitPlayed_.clear();

    // トレイル停止
    if (pPlayer_) {
        pPlayer_->GetRightHand()->StopTrailEmit();
        pPlayer_->GetLeftHand()->StopTrailEmit();
    }

    // 振動の状態をリセット
    isVibrationPlayed_ = false;
    vibrationTimer_    = 0.0f;
    isVibrating_       = false;
    previousLoopCount_ = 0;

    // 振動を停止
    size_t numGamepads = KetaEngine::Input::GetNumberOfJoysticks();
    for (size_t padNo = 0; padNo < numGamepads; ++padNo) {
        KetaEngine::Input::SetVibration(static_cast<int32_t>(padNo), 0.0f, 0.0f);
    }
}

void PlayerAttackRendition::Update(float deltaTime) {
    if (!pPlayer_ || !playerComboAttackData_) {
        return;
    }

    if (!isRendition_) {
        PlayRendition();
        isRendition_ = true;
    }

    currentTime_ += deltaTime;

    auto& renditionData = playerComboAttackData_->GetRenditionData();
    bool hasHit         = pPlayer_->GetPlayerCollisionInfo()->GetIsHit();

    // 通常演出の更新
    UpdateNormalRenditions(renditionData);

    
    bool isNewHit = hasHit && !previousHasHit_;
    if (isNewHit) {
        // ヒット時演出のフラグをリセットして再度発動可能にする
        isPlayedOnHit_.fill(false);
        UpdateHitRenditions(renditionData);
        hasTriggeredHitEffects_ = true;
    }
    // 前フレームのヒット状態を更新
    previousHasHit_ = hasHit;

    // オブジェクトアニメーションの更新
    UpdateObjectAnimations(renditionData);

    // 振動の更新
    UpdateVibration(renditionData, hasHit, deltaTime);
}

void PlayerAttackRendition::UpdateNormalRenditions(const PlayerAttackRenditionData& renditionData) {
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        // ヒット音はヒット時のみ再生するためスキップ
        if (static_cast<PlayerAttackRenditionData::Type>(i) == PlayerAttackRenditionData::Type::AudioHit) {
            continue;
        }
        // ポストエフェクトはリストで管理するためスキップ
        if (static_cast<PlayerAttackRenditionData::Type>(i) == PlayerAttackRenditionData::Type::PostEffect) {
            continue;
        }

        const auto& param = renditionData.GetRenditionParamFromIndex(i);

        // すでに再生済みならスキップ
        if (isPlayed_[i]) {
            continue;
        }

        // startTiming に達したら発動
        if (currentTime_ >= param.startTiming && param.fileName != "" && param.fileName != "None") {
            PlayRenditionEffect(static_cast<PlayerAttackRenditionData::Type>(i), param);
            isPlayed_[i] = true;
        }
    }

    // ポストエフェクトリストの更新
    const auto& postEffectList = renditionData.GetPostEffectList();
    if (isPostEffectPlayed_.size() != postEffectList.size()) {
        isPostEffectPlayed_.assign(postEffectList.size(), false);
    }
    for (size_t i = 0; i < postEffectList.size(); ++i) {
        if (isPostEffectPlayed_[i]) continue;
        const auto& param = postEffectList[i];
        if (currentTime_ >= param.startTiming && !param.fileName.empty() && param.fileName != "None") {
            PlayRenditionEffect(PlayerAttackRenditionData::Type::PostEffect, param);
            isPostEffectPlayed_[i] = true;
        }
    }
}

void PlayerAttackRendition::UpdateHitRenditions(const PlayerAttackRenditionData& renditionData) {
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        // ポストエフェクトはリストで管理するためスキップ
        if (static_cast<PlayerAttackRenditionData::Type>(i) == PlayerAttackRenditionData::Type::PostEffect) {
            continue;
        }

        const auto& param = renditionData.GetRenditionParamOnHitFromIndex(i);

        // すでに再生済みならスキップ
        if (isPlayedOnHit_[i]) {
            continue;
        }

        // startTiming に達したら発動
        if (currentTime_ >= param.startTiming && param.fileName != "" && param.fileName != "None") {
            PlayRenditionEffect(static_cast<PlayerAttackRenditionData::Type>(i), param);
            isPlayedOnHit_[i] = true;
        }
    }

    // ヒット時ポストエフェクトリストの更新
    const auto& postEffectOnHitList = renditionData.GetPostEffectOnHitList();
    if (isPostEffectOnHitPlayed_.size() != postEffectOnHitList.size()) {
        isPostEffectOnHitPlayed_.assign(postEffectOnHitList.size(), false);
    }
    for (size_t i = 0; i < postEffectOnHitList.size(); ++i) {
        if (isPostEffectOnHitPlayed_[i]) continue;
        const auto& param = postEffectOnHitList[i];
        if (currentTime_ >= param.startTiming && !param.fileName.empty() && param.fileName != "None") {
            PlayRenditionEffect(PlayerAttackRenditionData::Type::PostEffect, param);
            isPostEffectOnHitPlayed_[i] = true;
        }
    }
}

void PlayerAttackRendition::PlayRenditionEffect(PlayerAttackRenditionData::Type type, const PlayerAttackRenditionData::RenditionParam& param) {
    switch (type) {
    case PlayerAttackRenditionData::Type::CameraAction:
        pPlayer_->GetGameCamera()->PlayAnimation(param.fileName, param.isCameraReset);
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

    case PlayerAttackRenditionData::Type::ParticleEffect:
        pPlayer_->GetEffects()->Emit(param.fileName);
        break;

    case PlayerAttackRenditionData::Type::RibbonTrailEffect:
        pPlayer_->GetRightHand()->StartTrailEmit(param.fileName, "Player");
        pPlayer_->GetLeftHand()->StartTrailEmit(param.fileName, "Player");
        break;

    case PlayerAttackRenditionData::Type::AudioAttack:
    case PlayerAttackRenditionData::Type::AudioHit:
        KetaEngine::Audio::GetInstance()->Play(param.fileName + ".mp3", param.volume);
        break;

    default:
        break;
    }
}

void PlayerAttackRendition::UpdateObjectAnimations(const PlayerAttackRenditionData& renditionData) {
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        // すでに再生済みならスキップ
        if (isObjAnimePlayed_[i]) {
            continue;
        }

        // ファイル名が空ならスキップ
        if (param.fileName.empty() || param.fileName == "None") {
            isObjAnimePlayed_[i] = true;
            continue;
        }

        // startTiming に達したら発動
        if (currentTime_ >= param.startTiming) {
            switch (static_cast<PlayerAttackRenditionData::ObjAnimationType>(i)) {
            case PlayerAttackRenditionData::ObjAnimationType::Head:
                pPlayer_->GetObject3D()->transform_.PlayObjEaseAnimation(param.fileName, "Player");
                break;

            case PlayerAttackRenditionData::ObjAnimationType::RightHand:
                pPlayer_->GetRightHand()->GetObject3D()->transform_.PlayObjEaseAnimation(param.fileName, "RightHand");
                if (!param.trailFileName.empty() && param.trailFileName != "None") {
                    pPlayer_->GetRightHand()->StartTrailEmit(param.trailFileName, "Player");
                }
                break;

            case PlayerAttackRenditionData::ObjAnimationType::LeftHand:
                pPlayer_->GetLeftHand()->GetObject3D()->transform_.PlayObjEaseAnimation(param.fileName, "LeftHand");
                if (!param.trailFileName.empty() && param.trailFileName != "None") {
                    pPlayer_->GetLeftHand()->StartTrailEmit(param.trailFileName, "Player");
                }
                break;

            case PlayerAttackRenditionData::ObjAnimationType::MainHead:
                pPlayer_->GetPlayerAnimator().PlayMainHeadAnimation(param.fileName);
                break;

            default:
                break;
            }

            // 再生済みに設定
            isObjAnimePlayed_[i] = true;
        }
    }
}

void PlayerAttackRendition::UpdateVibration(const PlayerAttackRenditionData& renditionData, bool hasHit, float deltaTime) {
    const auto& vibParam = renditionData.GetVibrationParam();

    // ループカウントが増えた（クールタイム終了）場合、振動フラグをリセット
    int32_t currentLoopCount = pPlayer_->GetPlayerCollisionInfo()->GetCurrentLoopCount();
    if (currentLoopCount > previousLoopCount_) {
        isVibrationPlayed_ = false;
        previousLoopCount_ = currentLoopCount;
    }

    // トリガー条件のチェック
    bool shouldTrigger = !vibParam.triggerByHit || (vibParam.triggerByHit && hasHit);

    if (!shouldTrigger) {
        return;
    }

    // すでに再生済みでない場合
    if (!isVibrationPlayed_) {
        // startTiming に達したら発動
        if (currentTime_ >= vibParam.startTiming && vibParam.intensity > 0.0f) {
            KetaEngine::Input::SetVibration(0, vibParam.intensity, vibParam.intensity);

            // 再生済みに設定
            isVibrationPlayed_ = true;
            isVibrating_       = true;
            vibrationTimer_    = 0.0f;
        }
    }

    // 振動中の場合、タイマーを更新して停止判定
    if (isVibrating_) {
        vibrationTimer_ += deltaTime;

        if (vibrationTimer_ >= vibParam.duration) {
            // 振動を停止
            KetaEngine::Input::SetVibration(0, 0.0f, 0.0f);
            isVibrating_ = false;
        }
    }
}

void PlayerAttackRendition::PlayRendition() {
    // ごり押し演出追加処理
    std::string name = playerComboAttackData_->GetGroupName();

    if (name == "RushAttack") {
        pPlayer_->GetEffects()->RushAttackRingEffectEmit();
    }
}