#pragma once

#include "Editor/Easing/Easing.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include "Player/ComboCreator/PlayerAttackRenditionData.h"
#include "Player/ComboCreator/Timeline/PlayerComboAttackTimelinePhase.h"
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
    AttackTimelinePhase phase_                    = AttackTimelinePhase::MAIN;

    float currentTime_ = 0.0f;

    // 通常演出の再生フラグ
    std::array<std::vector<bool>, static_cast<size_t>(PlayerAttackRenditionData::Type::Count)> isPlayed_{};

    // ヒット時演出の再生フラグ
    std::array<std::vector<bool>, static_cast<size_t>(PlayerAttackRenditionData::Type::Count)> isPlayedOnHit_{};

    std::array<std::vector<bool>, static_cast<size_t>(PlayerAttackRenditionData::ObjAnimationType::Count)> isObjAnimePlayed_{};

    // ヒットエフェクトがトリガーされたかどうか
    bool hasTriggeredHitEffects_ = false;
    // 前フレームのヒット状態
    bool previousHasHit_ = false;

    // 振動関連
    std::vector<bool> isVibrationPlayed_;
    std::vector<float> vibrationTimer_;
    std::vector<bool> isVibrating_;
    int32_t previousLoopCount_ = 0;

    // ダメージヒット検出
    int32_t previousDamageHitCount_ = 0;

public:
    PlayerAttackRendition()  = default;
    ~PlayerAttackRendition() { Reset(); } // 破棄時にトレイルを確実に停止

    void Init(Player* player, PlayerComboAttackData* playerComboAttackData, AttackTimelinePhase phase = AttackTimelinePhase::MAIN);
    void Update(float deltaTime);
    void Reset();

private:
    void UpdateNormalRenditions(const PlayerAttackRenditionData& renditionData);
    void UpdateHitRenditions(const PlayerAttackRenditionData& renditionData);
    void UpdateDamageHitRenditions(const PlayerAttackRenditionData& renditionData);
    void PlayRenditionEffect(PlayerAttackRenditionData::Type type, const PlayerAttackRenditionData::RenditionParam& param);
    void UpdateObjectAnimations(const PlayerAttackRenditionData& renditionData);
    void UpdateVibration(const PlayerAttackRenditionData& renditionData, bool hasHit, bool isNewDamageHit, float deltaTime);

};