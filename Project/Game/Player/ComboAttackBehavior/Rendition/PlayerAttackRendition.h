#pragma once

#include "Editor/RailEditor/RailManager.h"
#include "Player/ComboCreator/PlayerAttackRenditionData.h"
#include "Easing/Easing.h"

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

    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::Type::Count)> isPlayed_{};
    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::ObjAnimationType::Count)> isObjAnimPlayed_{};

public:
    PlayerAttackRendition()  = default;
    ~PlayerAttackRendition() = default;

    void Init(Player* player, PlayerComboAttackData* playerComboAttackData);
    void Update(float deltaTime);
    void Reset();

    // ごり押し演出再生
    bool isRendition_;
    bool isBlur_;
    Easing<float> rushBlurEase_;
    float tempBlurParam_;
    void PlayRendition();

};