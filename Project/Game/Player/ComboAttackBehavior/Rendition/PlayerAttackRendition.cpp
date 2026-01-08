#include "PlayerAttackRendition.h"
#include "AttackEffect/AttackEffect.h"
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
    currentTime_ = 0.0f;
    isPlayed_.fill(false);
    isObjAnimPlayed_.fill(false);
    isAudioPlayed_.fill(false);

    // 振動の状態をリセット
    isVibrationPlayed_ = false;
    vibrationTimer_    = 0.0f;
    isVibrating_       = false;

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

    if (isBlur_) {
        rushBlurEase_.Update(KetaEngine::Frame::DeltaTime());
        KetaEngine::PostEffectRenderer::GetInstance()->GetRadialBlur()->SetBlurWidth(tempBlurParam_);
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

            case PlayerAttackRenditionData::Type::ParticleEffect:
                pPlayer_->GetEffects()->Emit(param.fileName);
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

    // オーディオの更新
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::AudioType::Count); ++i) {
        const auto& param = renditionData.GetAudioParamFromIndex(i);

        // トリガー条件がヒットの場合のcontinue処理
        if (param.triggerByHit && !pPlayer_->GetPlayerCollisionInfo()->GetIsHit()) {
            continue;
        }

        // すでに再生済みならスキップ
        if (isAudioPlayed_[i]) {
            continue;
        }

        // ファイル名が空ならスキップ
        if (param.fileName.empty() || param.fileName == "None") {
            isAudioPlayed_[i] = true;
            continue;
        }

        // startTiming に達したら発動
        if (currentTime_ >= param.startTiming) {
            KetaEngine::Audio::GetInstance()->Play(param.fileName + ".wav", param.volume);

            // 再生済みに設定
            isAudioPlayed_[i] = true;
        }
    }

    // 振動の更新
    const auto& vibParam = renditionData.GetVibrationParam();

    // トリガー条件がヒットの場合のcontinue処理
    if (vibParam.triggerByHit && !pPlayer_->GetPlayerCollisionInfo()->GetIsHit()) {
        // ヒット条件を満たさない場合は処理をスキップ
    } else {
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

    if (name == "FallRandingAttack") {
        pPlayer_->GetEffects()->FallEffectRenditionInit();
    }

    if (name == "RushAttack") {
        rushBlurEase_.Init("RushBlur.json");
        rushBlurEase_.SetAdaptValue(&tempBlurParam_);
        isBlur_ = true;
        KetaEngine::PostEffectRenderer::GetInstance()->SetPostEffectMode(KetaEngine::PostEffectMode::RADIALBLUR);

        rushBlurEase_.SetOnWaitEndCallback([this]() {
            KetaEngine::PostEffectRenderer::GetInstance()->SetPostEffectMode(KetaEngine::PostEffectMode::NONE);
        });
        pPlayer_->GetEffects()->RushAttackRingEffectEmit();
    }
}