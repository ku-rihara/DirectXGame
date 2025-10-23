#pragma once
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "Vector3.h"
#include <json.hpp>
#include <string>
#include <vector>

/// <summary>
/// 制御点マネージャー
/// </summary>
class ControlPosManager {

private:
    using json = nlohmann::json;

private:
    std::vector<WorldTransform> transforms_;
    std::vector<Vector3> movePosies_;
    std::vector<std::unique_ptr<Object3d>> obj3ds_;
    std::string filePath_;
    Vector3 tempAddPosition_;
    WorldTransform* parentTransform_;

    bool isDraw_ = false;

    const std::string dyrectrypath_ = "Resources/ControlPoint/";

public:
    ControlPosManager()  = default;
    ~ControlPosManager() = default;

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="Direction">方向</param>
    /// <param name="isDraw">描画フラグ</param>
    void Update(const Vector3& Direction = {1.0f, 1.0f, 1.0f}, const bool& isDraw = false);

    /// <summary>
    /// 親トランスフォームの設定
    /// </summary>
    /// <param name="parent">親トランスフォーム</param>
    void SetParent(WorldTransform* parent);

    /// <summary>
    /// 制御点の追加
    /// </summary>
    /// <param name="position">座標</param>
    void AddPoint(const Vector3& position);

    /// <summary>
    /// 制御点の削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemovePoint(size_t index);

    /// <summary>
    /// ファイルへ保存
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void SaveToFile(const std::string& filename);

    /// <summary>
    /// ファイルから読み込み
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void LoadFromFile(const std::string& filename);

    /// <summary>
    /// ImGui更新
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void ImGuiUpdate(const std::string& filename);

public:
    std::vector<Vector3> GetWorldPositions() const;
    std::vector<Vector3> GetLocalPositions() const;

    void SetIsDraw(const bool& is) { isDraw_ = is; }
};