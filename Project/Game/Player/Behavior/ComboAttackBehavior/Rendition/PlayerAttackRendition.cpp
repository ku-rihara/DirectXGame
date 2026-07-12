#include "PlayerAttackRendition.h"
#include <algorithm>
#include "Audio/Audio.h"
#include "GameCamera/GameCamera.h"
#include "Input/Input.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"

void PlayerAttackRendition::Init(Player* player, PlayerComboAttackData* playerComboAttackData, AttackTimelinePhase phase) {
    pPlayer_               = player;
    playerComboAttackData_ = playerComboAttackData;
    phase_                 = phase;
    Reset();
}

void PlayerAttackRendition::Reset() {
    currentTime_ = 0.0f;
    for (auto& played : isPlayed_) {
        played.clear();
    }
    for (auto& played : isPlayedOnHit_) {
        played.clear();
    }
    for (auto& played : isObjAnimePlayed_) {
        played.clear();
    }
    hasTriggeredHitEffects_ = false;
    previousHasHit_         = false;

    // トレイル停止
    if (pPlayer_) {
        pPlayer_->GetRightHand()->StopTrailEmit();
        pPlayer_->GetLeftHand()->StopTrailEmit();
        pPlayer_->GetPlayerAnimator().StopMainHeadTrailEmit();
    }

    // 振動の状態をリセット
    isVibrationPlayed_.clear();
    vibrationTimer_.clear();
    isVibrating_.clear();
    previousLoopCount_      = 0;
    previousDamageHitCount_ = 0;

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

    currentTime_ += deltaTime;

    auto& renditionData = playerComboAttackData_->GetRenditionDataForPhase(phase_);
    bool hasHit         = pPlayer_->GetPlayerCollisionInfo()->GetIsHit();

    // ダメージヒット検出
    int32_t currentDamageHitCount = pPlayer_->GetPlayerCollisionInfo()->GetDamageHitCount();
    bool isNewDamageHit = (currentDamageHitCount != previousDamageHitCount_);

    // 通常演出の更新
    UpdateNormalRenditions(renditionData);

    bool isNewHit = hasHit && !previousHasHit_;
    if (isNewHit) {
        // ヒット時演出のフラグをリセットして再度発動可能にする
        for (auto& played : isPlayedOnHit_) {
            played.clear();
        }
        UpdateHitRenditions(renditionData);
        hasTriggeredHitEffects_ = true;
    }
    // 前フレームのヒット状態を更新
    previousHasHit_ = hasHit;

    // ダメージヒット時演出
    if (isNewDamageHit) {
        UpdateDamageHitRenditions(renditionData);
        previousDamageHitCount_ = currentDamageHitCount;
    }

    // オブジェクトアニメーションの更新
    UpdateObjectAnimations(renditionData);

    // 振動の更新
    UpdateVibration(renditionData, hasHit, isNewDamageHit, deltaTime);
}

void PlayerAttackRendition::UpdateNormalRenditions(const PlayerAttackRenditionData& renditionData) {
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        auto type = static_cast<PlayerAttackRenditionData::Type>(i);

        // ヒット音はヒット時のみ再生するためスキップ
        if (type == PlayerAttackRenditionData::Type::AudioHit) {
            continue;
        }

        const auto& list = renditionData.GetRenditionListFromType(type);
        auto& played      = isPlayed_[i];
        if (played.size() != list.size()) {
            played.assign(list.size(), false);
        }

        for (size_t j = 0; j < list.size(); ++j) {
            if (played[j]) continue;
            const auto& param = list[j];
            if (currentTime_ >= param.startTiming && !param.fileName.empty() && param.fileName != "None") {
                PlayRenditionEffect(type, param);
                played[j] = true;
            }
        }
    }
}

void PlayerAttackRendition::UpdateHitRenditions(const PlayerAttackRenditionData& renditionData) {
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        auto type = static_cast<PlayerAttackRenditionData::Type>(i);

        const auto& list = renditionData.GetRenditionOnHitListFromType(type);
        auto& played      = isPlayedOnHit_[i];
        if (played.size() != list.size()) {
            played.assign(list.size(), false);
        }

        for (size_t j = 0; j < list.size(); ++j) {
            if (played[j]) continue;
            const auto& param = list[j];
            if (currentTime_ >= param.startTiming && !param.fileName.empty() && param.fileName != "None") {
                PlayRenditionEffect(type, param);
                played[j] = true;
            }
        }
    }
}

void PlayerAttackRendition::PlayRenditionEffect(PlayerAttackRenditionData::Type type, const PlayerAttackRenditionData::RenditionParam& param) {
    switch (type) {
    case PlayerAttackRenditionData::Type::CameraAction:
        pPlayer_->GetContext().pGameCamera->PlayAnimation(param.fileName, param.isCameraReset);
        break;

    case PlayerAttackRenditionData::Type::HitStop:
        pPlayer_->GetEffects()->PlayHitStop(param.fileName);
        break;

    case PlayerAttackRenditionData::Type::ShakeAction:
        pPlayer_->GetContext().pGameCamera->PlayShake(param.fileName);
        break;

    case PlayerAttackRenditionData::Type::PostEffect:
        pPlayer_->GetEffects()->PlayPostEffect(param.fileName);
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
        auto animType = static_cast<PlayerAttackRenditionData::ObjAnimationType>(i);

        const auto& list = renditionData.GetObjAnimationListFromType(animType);
        auto& played      = isObjAnimePlayed_[i];
        if (played.size() != list.size()) {
            played.assign(list.size(), false);
        }

        for (size_t j = 0; j < list.size(); ++j) {
            if (played[j]) continue;
            const auto& param = list[j];

            // ファイル名が空ならスキップ（再生済み扱いにはしない）
            if (param.fileName.empty() || param.fileName == "None") {
                played[j] = true;
                continue;
            }

            // startTiming に達したら発動
            if (currentTime_ >= param.startTiming) {
                switch (animType) {
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
                    if (!param.trailFileName.empty() && param.trailFileName != "None") {
                        pPlayer_->GetPlayerAnimator().StartMainHeadTrailEmit(param.trailFileName, "Player");
                    }
                    break;

                default:
                    break;
                }

                played[j] = true;
            }
        }
    }
}

void PlayerAttackRendition::UpdateDamageHitRenditions(const PlayerAttackRenditionData& renditionData) {
    // OnHit演出の中でrepeatOnDamage=trueのものをダメージごとに再生
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        auto type = static_cast<PlayerAttackRenditionData::Type>(i);
        for (const auto& param : renditionData.GetRenditionOnHitListFromType(type)) {
            if (param.repeatOnDamage && !param.fileName.empty() && param.fileName != "None") {
                PlayRenditionEffect(type, param);
            }
        }
    }
}

void PlayerAttackRendition::UpdateVibration(const PlayerAttackRenditionData& renditionData, bool hasHit, bool isNewDamageHit, float deltaTime) {
    const auto& list = renditionData.GetVibrationList();

    if (isVibrationPlayed_.size() != list.size()) {
        isVibrationPlayed_.assign(list.size(), false);
        vibrationTimer_.assign(list.size(), 0.0f);
        isVibrating_.assign(list.size(), false);
    }

    // クールタイム終了した場合、振動再生フラグを全てリセット
    int32_t currentLoopCount = pPlayer_->GetPlayerCollisionInfo()->GetCurrentLoopCount();
    if (currentLoopCount > previousLoopCount_) {
        std::fill(isVibrationPlayed_.begin(), isVibrationPlayed_.end(), false);
        previousLoopCount_ = currentLoopCount;
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto& vibParam = list[i];

        if (vibParam.repeatOnDamage) {
            // ダメージヒットごとに振動する
            if (isNewDamageHit && vibParam.intensity > 0.0f) {
                isVibrating_[i]    = true;
                vibrationTimer_[i] = 0.0f;
            }
        } else {
            // 通常：トリガー条件チェックのうえ1回だけ振動
            bool shouldTrigger = !vibParam.triggerByHit || (vibParam.triggerByHit && hasHit);
            if (shouldTrigger && !isVibrationPlayed_[i]) {
                if (currentTime_ >= vibParam.startTiming && vibParam.intensity > 0.0f) {
                    isVibrationPlayed_[i] = true;
                    isVibrating_[i]       = true;
                    vibrationTimer_[i]    = 0.0f;
                }
            }
        }

        // 振動中のタイマー管理
        if (isVibrating_[i]) {
            vibrationTimer_[i] += deltaTime;
            float effectiveDuration = (vibParam.duration > 0.0f) ? vibParam.duration : 0.1f;
            if (vibrationTimer_[i] >= effectiveDuration) {
                isVibrating_[i] = false;
            }
        }
    }

    // 実際のゲームパッド振動は、同時に振動中のエントリの中で最大強度のものを採用する
    float maxIntensity = 0.0f;
    for (size_t i = 0; i < list.size(); ++i) {
        if (isVibrating_[i]) {
            maxIntensity = (std::max)(maxIntensity, list[i].intensity);
        }
    }

    size_t numGamepads = KetaEngine::Input::GetNumberOfJoysticks();
    for (size_t padNo = 0; padNo < numGamepads; ++padNo) {
        KetaEngine::Input::SetVibration(static_cast<int32_t>(padNo), maxIntensity, maxIntensity);
    }
}
