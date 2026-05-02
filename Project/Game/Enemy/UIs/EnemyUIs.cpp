#include "EnemyUIs.h"
#include "HPBar/EnemyHPBarColorConfig.h"

void EnemyUIs::Init(float hpMax) {
    hpBar_ = std::make_unique<EnemyHPBar>();
    hpBar_->Init(hpMax);

    groupIcon_ = std::make_unique<GroupIcon>();
    groupIcon_->Init();
}

void EnemyUIs::Update(float hp, const Vector2& baseScreenPos, bool isDraw) {
    Vector2 hpPos = baseScreenPos - hpBarOffset_;

    hpBar_->SetIsDraw(isDraw);
    hpBar_->SetPosition(hpPos);
    hpBar_->Update(hp);
}

void EnemyUIs::UpdateGroupIcon(const Vector2& screenPos, bool isDraw) {
    groupIcon_->Update(screenPos, isDraw);
}

void EnemyUIs::Hide(float hp) {
    hpBar_->SetIsDraw(false);
    hpBar_->Update(hp);
}

void EnemyUIs::SetGroupIndex(int32_t index) {
    groupIcon_->SetGroupIndex(index);
}

void EnemyUIs::SetColorConfig(EnemyHPBarColorConfig* config) {
    hpBar_->SetColorConfig(config);
}

void EnemyUIs::SetHPBarOffset(const Vector2& offset) {
    hpBarOffset_ = offset;
}

void EnemyUIs::SetGroupIconOffset(const Vector2& offset) {
    groupIconOffset_ = offset;
}