#pragma once
#include "Line3D/Line3D.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "Vector3.h"
#include <memory>
#include <vector>

#include "utility/RailEditor/ControlPosManager.h"
#include "utility/RailEditor/Rail.h"

class SrvManager;

/// <summary>
/// レールマネージャー
/// </summary>
class RailManager {
public:
    enum class PositionMode {
        WORLD,
        LOCAL,
    };

public:
    RailManager()  = default;
    ~RailManager() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="groupName">グループ名</param>
    void Init(const std::string& groupName);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speed">移動速度</param>
    /// <param name="mode">座標モード</param>
    /// <param name="direction">方向</param>
    void Update(const float& speed, const PositionMode& mode = PositionMode::WORLD, const Vector3& direction = {1.0f, 1.0f, 1.0f});

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    /// <param name="size">サイズ</param>
    void SetCubeLine(const Vector3& size = Vector3(0.1f, 0.1f, 0.1f));

    void RoopOrStop(); //< ループまたは停止
    void ImGuiEdit(); //< ImGuiにて編集

private:
    std::string groupName_;

    Rail rail_;
    std::unique_ptr<ControlPosManager> emitControlPosManager_;
    WorldTransform worldTransform_;
    ViewProjection viewProjection_;

    float railMoveTime_ = 0.0f;
    Vector3 cameraRotate_;
    Vector3 scale_;
    bool isRoop_;

    std::unique_ptr<Line3D> lineDrawer_;

public:

    // getter
    bool GetIsRoop() const { return isRoop_; }
    float GetRailMoveTime() const { return railMoveTime_; }
    const Vector3& GetCameraRotate() const { return cameraRotate_; }
    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
    const ViewProjection& GetViewProjection() const { return viewProjection_; }
    Vector3 GetPositionOnRail() const;

    // setter
    void SetParent(WorldTransform* parent);
    void SetScale(Vector3 scale) { scale_ = scale; }
    void SetRailMoveTime(const float& t) { railMoveTime_ = t; }
    void SetIsRoop(const bool& is) { isRoop_ = is; }
    void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(float value) { viewProjection_.farZ_ = value; }
};