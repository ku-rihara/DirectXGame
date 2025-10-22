#pragma once
#include <memory>
#include <unordered_set>
#include <vector>

class Object3d;
class ViewProjection;

/// <summary>
/// Object3d登録管理クラス
/// </summary>
class Object3DRegistry {
private:
    Object3DRegistry() = default;
    ~Object3DRegistry();

    Object3DRegistry(const Object3DRegistry&)            = delete;
    Object3DRegistry& operator=(const Object3DRegistry&) = delete;

public:
    static Object3DRegistry* GetInstance(); //< シングルトンインスタンス取得

    /// <summary>
    /// オブジェクト登録
    /// </summary>
    /// <param name="object">登録するオブジェクト</param>
    void RegisterObject(Object3d* object);

    /// <summary>
    /// オブジェクト登録解除
    /// </summary>
    /// <param name="object">解除するオブジェクト</param>
    void UnregisterObject(Object3d* object);

    void UpdateAll(); //< 全オブジェクト更新

    /// <summary>
    /// 全オブジェクト描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawAll(const ViewProjection& viewProjection);

    /// <summary>
    /// 全オブジェクトのシャドウ描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawAllShadow(const ViewProjection& viewProjection);

    void Clear();      //< 全オブジェクトクリア
    void DebugImGui(); //< ImGuiデバッグ表示

private:
    static bool isDestroyed_;

    std::unordered_set<Object3d*> objects_;
    static Object3DRegistry* instance_;

public:
    const size_t& GetRegisteredCount() const { return objects_.size(); }
};