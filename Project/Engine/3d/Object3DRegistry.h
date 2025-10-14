#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

// 前方宣言
class Object3d;
class ViewProjection;

class Object3DRegistry {
public:
  
    static Object3DRegistry* GetInstance();

    /// ============================================================
    /// public methods
    /// ============================================================

    /// オブジェクト登録、解除
    void RegisterObject(Object3d* object);
    void UnregisterObject(Object3d* object);

    /// 更新、描画
    void UpdateAll();
    void DrawAll(const ViewProjection& viewProjection);
    void DrawAllShadow(const ViewProjection& viewProjection);

    void Clear();
    void DebugImGui();

private:
    Object3DRegistry()  = default;
    ~Object3DRegistry();

    /// コピー禁止
    Object3DRegistry(const Object3DRegistry&)            = delete;
    Object3DRegistry& operator=(const Object3DRegistry&) = delete;

    /// ============================================================
    /// private members
    /// ============================================================
private:
    static bool isDestroyed_;

    std::unordered_set<Object3d*> objects_;
    static Object3DRegistry* instance_;

public:
    // getter
    const size_t& GetRegisteredCount() const { return objects_.size(); }
};