#pragma once

#include "Easing/Easing.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include "Player/ComboCreator/PlayerAttackRenditionData.h"
#include <vector>

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

    // ポストエフェクトリスト用再生フラグ
    std::vector<bool> isPostEffectPlayed_;
    std::vector<bool> isPostEffectOnHitPlayed_;

    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::ObjAnimationType::Count)> isObjAnimePlayed_{};

    // ヒットエフェクトがトリガーされたかどうか
    bool hasTriggeredHitEffects_ = false;
    // 前フレームのヒット状態（マルチヒット検出用）
    bool previousHasHit_ = false;

    // 振動関連
    bool isVibrationPlayed_ = false;
    float vibrationTimer_   = 0.0f;
    bool isVibrating_       = false;
    int32_t previousLoopCount_ = 0;

    // ダメージヒット検出（連続演出用）
    int32_t previousDamageHitCount_ = 0;

public:
    PlayerAttackRendition()  = default;
    ~PlayerAttackRendition() = default;

    void Init(Player* player, PlayerComboAttackData* playerComboAttackData);
    void Update(float deltaTime);
    void Reset();

    // ごり押し演出再生
    bool isRendition_;

private:
    void UpdateNormalRenditions(const PlayerAttackRenditionData& renditionData);
    void UpdateHitRenditions(const PlayerAttackRenditionData& renditionData);
    void UpdateDamageHitRenditions(const PlayerAttackRenditionData& renditionData);
    void PlayRenditionEffect(PlayerAttackRenditionData::Type type, const PlayerAttackRenditionData::RenditionParam& param);
    void UpdateObjectAnimations(const PlayerAttackRenditionData& renditionData);
    void UpdateVibration(const PlayerAttackRenditionData& renditionData, bool hasHit, bool isNewDamageHit, float deltaTime);

public:
    void PlayRendition();
};