#pragma once

#include "Easing/Easing.h"
#include "Editor/RailEditor/RailManager.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include "Player/ComboCreator/PlayerAttackRenditionData.h"

class Player;
class PlayerComboAttackData;

/// <summary>
/// プレイヤー攻撃演出制御クラス
/// </summary>
class PlayerAttackRendition {
private:
    Player* pPlayer_                              = nullptr;
    PlayerComboAttackData* playerComboAttackData_ = nullptr;

    float currentTime_ = 0.0f;

    // 通常演出の再生フラグ
    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::Type::Count)> isPlayed_{};

    // ヒット時演出の再生フラグ
    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::Type::Count)> isPlayedOnHit_{};

    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::ObjAnimationType::Count)> isObjAnimePlayed_{};

    // ヒットエフェクトがトリガーされたかどうか
    bool hasTriggeredHitEffects_ = false;
    // 前フレームのヒット状態（マルチヒット検出用）
    bool previousHasHit_ = false;

    // 振動関連
    bool isVibrationPlayed_ = false;
    float vibrationTimer_   = 0.0f;
    bool isVibrating_       = false;

public:
    PlayerAttackRendition()  = default;
    ~PlayerAttackRendition() = default;

    void Init(Player* player, PlayerComboAttackData* playerComboAttackData);
    void Update(float deltaTime);
    void Reset();

    // ごり押し演出再生
    bool isRendition_;
    bool isBlur_;
    KetaEngine::Easing<float> rushBlurEase_;
    float tempBlurParam_;

private:
    void UpdateNormalRenditions(const PlayerAttackRenditionData& renditionData);
    void UpdateHitRenditions(const PlayerAttackRenditionData& renditionData);
    void PlayRenditionEffect(PlayerAttackRenditionData::Type type, const PlayerAttackRenditionData::RenditionParam& param);
    void UpdateObjectAnimations(const PlayerAttackRenditionData& renditionData);
    void UpdateVibration(const PlayerAttackRenditionData& renditionData, bool hasHit, float deltaTime);

public:
    void PlayRendition();
};