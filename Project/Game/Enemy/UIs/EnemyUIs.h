#pragma once
#include "GroupIcon/GroupIcon.h"
#include "HPBar/EnemyHPBar.h"
#include "Vector2.h"
#include <memory>

class EnemyHPBarColorConfig;

/// <summary>
/// 敵1体分のUI管理クラス
/// </summary>
class EnemyUIs {
public:
    EnemyUIs()  = default;
    ~EnemyUIs() = default;

    void Init(float hpMax);

    /// <summary>
    /// 毎フレーム更新
    /// </summary>
    /// <param name="hp">現在HP</param>
    /// <param name="baseScreenPos">敵のスクリーン座標</param>
    /// <param name="isDraw">表示するか</param>
    void Update(float hp, const Vector2& baseScreenPos, bool isDraw);
    void UpdateGroupIcon(const Vector2& screenPos, bool isDraw);

    void Hide(float hp);

    void SetGroupIndex(int32_t index);
    void SetColorConfig(EnemyHPBarColorConfig* config);
    void SetHPBarOffset(const Vector2& offset);
    void SetGroupIconOffset(const Vector2& offset);

private:
    std::unique_ptr<EnemyHPBar> hpBar_;
    std::unique_ptr<GroupIcon> groupIcon_;

    Vector2 hpBarOffset_;
    Vector2 groupIconOffset_ = {0.0f, -24.0f}; // HPバー座標からの相対オフセット
};