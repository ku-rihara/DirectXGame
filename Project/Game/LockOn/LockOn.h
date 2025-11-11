#pragma once
#include "2d/Sprite.h"
#include "3d/ViewProjection.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

class BaseEnemy;
class Player;

/// <summary>
/// ロックオンシステムクラス
/// 敵の自動検索とターゲット切り替え
/// </summary>
class LockOn {
public:
    using LockOnVariant = std::variant<BaseEnemy*>;

public:
    struct TargetMarker {
        std::unique_ptr<Sprite> sprite;
        Vector2 screenPosition;
        bool isCurrentTarget = false;
    };

public:
    LockOn()  = default;
    ~LockOn() = default;

    // 初期化、更新
    void Init();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="targets">ターゲットリスト</param>
    /// <param name="player">プレイヤー</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Update(const std::vector<LockOnVariant>& targets, const Player* player, const ViewProjection& viewProjection);

    /// <summary>
    /// オブジェクト破壊時の処理
    /// </summary>
    /// <param name="obj">破壊されたオブジェクト</param>
    void OnObjectDestroyed(const LockOnVariant& obj);

    // editor
    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    /// <summary>
    /// 自動ターゲット検索
    /// </summary>
    /// <param name="targets">ターゲットリスト</param>
    /// <param name="player">プレイヤー</param>
    void AutoSearchTarget(const std::vector<LockOnVariant>& targets, const Player* player);

    /// <summary>
    /// ターゲット切り替え処理
    /// </summary>
    /// <param name="targets">ターゲットリスト</param>
    /// <param name="player">プレイヤー</param>
    void HandleTargetSwitching(const std::vector<LockOnVariant>& targets, const Player* player);

    /// <summary>
    /// 有効なターゲット取得
    /// </summary>
    /// <param name="targets">ターゲットリスト</param>
    /// <param name="player">プレイヤー</param>
    /// <returns>有効なターゲットリスト</returns>
    std::vector<LockOnVariant> GetValidTargets(const std::vector<LockOnVariant>& targets, const Player* player) const;

    /// <summary>
    /// ターゲット範囲チェック
    /// </summary>
    /// <param name="target">ターゲット</param>
    /// <param name="player">プレイヤー</param>
    /// <param name="relativePosition">相対位置</param>
    /// <returns>範囲内ならtrue</returns>
    bool IsTargetRange(const LockOnVariant& target, const Player* player, Vector3& relativePosition) const;

    /// <summary>
    /// ターゲットを距離順にソート
    /// </summary>
    /// <param name="validTargets">有効なターゲットリスト</param>
    void SortTargetsByDistance(std::vector<std::pair<float, LockOnVariant>>& validTargets) const;

    /// <summary>
    /// ターゲットを角度順にソート
    /// </summary>
    /// <param name="validTargets">有効なターゲットリスト</param>
    void SortTargetsByAngle(std::vector<std::pair<float, LockOnVariant>>& validTargets) const;

    /// <summary>
    /// ターゲットマーカーのリサイズ
    /// </summary>
    /// <param name="targetCount">ターゲット数</param>
    void ResizeTargetMarkers(const size_t& targetCount);

    /// <summary>
    /// 現在のレティクルUI更新
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void UpdateCurrentReticleUI(const ViewProjection& viewProjection);

    /// <summary>
    /// Lerp時間の増加
    /// </summary>
    /// <param name="incrementTime">増加時間</param>
    void LerpTimeIncrement(const float& incrementTime);

    /// <summary>
    /// ターゲットマーカー更新
    /// </summary>
    /// <param name="validTargets">有効なターゲットリスト</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
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
    float angleRange_; //< 角度範囲

    // 切り替えモード
    enum class SwitchMode {
        Distance, // 距離順
        Angle // 角度順
    };

    SwitchMode switchMode_ = SwitchMode::Angle;

public:
    // getter
    const bool& ExistTarget() const { return currentTarget_.has_value(); }
    Vector3 GetCurrentTargetPosition() const;
    const LockOnVariant* GetIsCurrentTarget() const;

    /// <summary>
    /// ターゲットオブジェクトの位置取得
    /// </summary>
    /// <param name="target">ターゲット</param>
    /// <returns>位置座標</returns>
    Vector3 GetTargetObjectPosition(const LockOnVariant& target) const;
};