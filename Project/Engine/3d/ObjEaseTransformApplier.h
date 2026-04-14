#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <memory>
#include <string>

namespace KetaEngine {

class ObjEaseAnimationPlayer;
class WorldTransform;

/// <summary>
/// WorldTransformに対するObjEaseAnimationの適用処理をするクラス
/// </summary>
class ObjEaseTransformApplier {
public:
    /// Apply() が返すオフセット結果
    struct OffsetResult {
        Vector3 scale             = Vector3::OneVector();
        Vector3 rotation          = Vector3::ZeroVector();
        Vector3 translation       = Vector3::ZeroVector();
        Quaternion quaternion     = Quaternion::Identity();
        Vector3 anchorScale       = Vector3::ZeroVector();
        Vector3 anchorRotation    = Vector3::ZeroVector();
        Vector3 anchorTranslation = Vector3::ZeroVector();
        bool overwriteDirectScale = false;
        Vector3 directScaleValue  = Vector3::OneVector();
    };

public:
    ObjEaseTransformApplier();
    ~ObjEaseTransformApplier();

    /// 初期化
    void Init();

    /// アニメーション再生
    void Play(
        const std::string& animationName, const std::string& categoryName, const Vector3& currentOffsetScale,
        const Vector3& currentOffsetRotation, const Vector3& currentOffsetTranslation, WorldTransform* railParent);

    /// アニメーション停止
    void Stop();

    /// 毎フレームの内部更新
    void Update();

    /// アニメーション値をOffsetResultに書き込む
    bool Apply(
        OffsetResult& result,
        const Vector3& worldRotation,
        const Quaternion& quaternion,
        bool isAdaptDirectScale,
        bool isQuaternionMode);

    /// Original値をresultに書き込む
    bool ApplyOriginalValues(OffsetResult& result);

    /// LookAt処理をresultに適用
    void ApplyLookAtDirection(
        OffsetResult& result,
        const Vector3& direction,
        bool isQuaternionMode,
        const Quaternion& quaternion);

    bool IsPlaying() const;

private:
    std::unique_ptr<ObjEaseAnimationPlayer> player_;

    bool applyOriginalOnStop_      = false;
    bool lookAtDirectionEnabled_   = true;
    bool reverseDirectionOnReturn_ = false;
    Vector3 lastPlayDirection_     = Vector3::ZeroVector();

public:
    ObjEaseAnimationPlayer* GetPlayer();

    void SetLookAtDirectionEnabled(bool enabled) { lookAtDirectionEnabled_ = enabled; }
    void SetReverseDirectionOnReturn(bool reverse) { reverseDirectionOnReturn_ = reverse; }
    void SetApplyOriginalOnStop(bool value) { applyOriginalOnStop_ = value; }
};

} // namespace KetaEngine
