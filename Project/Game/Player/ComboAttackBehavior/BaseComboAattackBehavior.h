#pragma once

#include "Easing.h"
#include <memory>
#include <string>

class Player;
class PlayerParameter;
class BaseComboAattackBehavior {
protected:
    std::string name_; // 振る舞い名
    Player* pPlayer_                   = nullptr; /// プレイヤー
    PlayerParameter* pPlayerParameter_ = nullptr;

    bool isNextCombo_;
    Easing startEasing_;

    /// motion
    float rotateValue_;
    float rotateValueAnti_;
    float rotateEaseT_;
    float tempRotateValue_;

    /// floatMotion
    float floatValue_;
    float tempFloatValue_;

    Easing floatEase_;

public:
    BaseComboAattackBehavior(const std::string& name, Player* player)
        : name_(name), pPlayer_(player), pPlayerParameter_(player->GetParamater()) {}
    
    virtual ~BaseComboAattackBehavior() {}

    virtual void Update() = 0; /// 更新
    virtual void Debug()  = 0; /// デバッグ

    void AnimationInit();

    void ScalingEaseUpdate();
    void RotateMotionUpdate(const float& start, const float& end, const bool& isClockwise);
    void FloatAnimationUpdate();

    virtual void PreOderNextComboForButton();
    virtual void ChangeNextCombo(std::unique_ptr<BaseComboAattackBehavior> nextCombo);

    float GetRotateValue() const { return rotateValue_; }
    float GetRotateValueAnti() const { return rotateValueAnti_; }
};