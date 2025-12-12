#pragma once
#include "3d/ViewProjection.h"
#include <memory>
#include <unordered_set>

namespace KetaEngine {

class Line3D;

/// <summary>
/// Line3Dの登録管理クラス
/// </summary>
class Line3DManager {
private:
    Line3DManager() = default;
    ~Line3DManager();

    Line3DManager(const Line3DManager&)            = delete;
    Line3DManager& operator=(const Line3DManager&) = delete;

public:
    static Line3DManager* GetInstance(); 

    /// <summary>
    /// Line3Dオブジェクト登録
    /// </summary>
    /// <param name="line">登録するLine3Dオブジェクト</param>
    void RegisterLine(Line3D* line);

    /// <summary>
    /// Line3Dオブジェクト登録解除
    /// </summary>
    /// <param name="line">解除するLine3Dオブジェクト</param>
    void UnregisterLine(Line3D* line);

    /// <summary>
    /// 全Line3Dオブジェクトの一括描画
    /// </summary>
    /// <param name="viewProj">ビュープロジェクション</param>
    void DrawAll(const ViewProjection& viewProj);

    /// <summary>
    /// 全Line3Dオブジェクトのリセット
    /// </summary>
    void ResetAll();

    void Clear(); //< 全Line3Dオブジェクトクリア

private:
    static bool isDestroyed_;
    std::unordered_set<Line3D*> lines_;

public:
    const size_t& GetRegisteredCount() const { return lines_.size(); }
};

}; // KetaEngine
