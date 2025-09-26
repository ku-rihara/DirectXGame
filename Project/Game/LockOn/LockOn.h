#pragma once
#include "2d/Sprite.h"
#include "3d/ViewProjection.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

// 前方宣言
class BaseEnemy;
class Player;

struct TargetMarker {
    std::unique_ptr<Sprite> sprite;
    Vector2 screenPosition;
    bool isCurrentTarget = false;
};

class LockOn {
public:
    using LockOnVariant = std::variant<BaseEnemy*>;

public:
    LockOn()  = default;
    ~LockOn() = default;

    // 初期化、更新、描画
    void Init();
    void Update(const std::vector<LockOnVariant>& targets, const Player* player, const ViewProjection& viewProjection);
    void Draw();

    void OnObjectDestroyed(const LockOnVariant& obj);

    // editor
    void AdjustParam();
    void BindParams();

private:
    // 自動検索関連
    void AutoSearchTarget(const std::vector<LockOnVariant>& targets, const Player* player);

    // 手動切り替え関連
    void HandleTargetSwitching(const std::vector<LockOnVariant>& targets, const Player* player);

    // ターゲット検索・管理
    std::vector<LockOnVariant> GetValidTargets(const std::vector<LockOnVariant>& targets, const Player* player) const;
    bool IsTargetRange(const LockOnVariant& target, const Player* player, Vector3& relativePosition) const;
    void SortTargetsByDistance(std::vector<std::pair<float, LockOnVariant>>& validTargets) const;
    void SortTargetsByAngle(std::vector<std::pair<float, LockOnVariant>>& validTargets) const;

    // UI更新
    void ResizeTargetMarkers(const size_t& targetCount);
    void UpdateCurrentReticleUI(const ViewProjection& viewProjection);
    void LerpTimeIncrement(float incrementTime);
    void UpdateTargetMarkers(const std::vector<LockOnVariant>& validTargets, const ViewProjection& viewProjection);

private:
    // global parameter
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "LockOn";

    // Sprite
    std::unique_ptr<Sprite> lockOnMark_;
    std::vector<TargetMarker> ableLockOnMarkers_;
    uint32_t reticleHandle_;
    Vector2 currentTargetScale_;
    Vector2 ableTargetScale_;

    // ターゲット
    std::optional<LockOnVariant> currentTarget_;
    std::vector<LockOnVariant> validTargets_;
    size_t currentTargetIndex_ = 0;

    // Sprite Param
    Vector2 prePos_;
    Vector2 spriteScale_;
    Vector2 lockOnMarkPos_;
    float spriteRotation_;

    // 自動検索設定
    bool autoSearchEnabled_   = true;
    float autoSearchInterval_ = 0.1f;
    float autoSearchTimer_    = 0.0f;

    // 切り替え
    float lerpTime_;
    bool prevSwitchInput_ = false;
    float targetChangeSpeed_;

    // パラメータ
    float minDistance_; //< 最小距離
    float maxDistance_; //< 最大距離
    float angleRange_; //< 角度範囲（度）

    // 切り替えモード
    enum class SwitchMode {
        Distance, // 距離順
        Angle // 角度順
    };

    SwitchMode switchMode_ = SwitchMode::Angle;

public:
    const bool& ExistTarget() const { return currentTarget_.has_value(); }
    Vector3 GetCurrentTargetPosition() const;
    Vector3 GetTargetObjectPosition(const LockOnVariant& target) const;
    const LockOnVariant* GetIsCurrentTarget() const;
};
