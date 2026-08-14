#pragma once
#include "Utility/Registry/RegistryBase.h"

namespace KetaEngine {

class Object3d;
class ViewProjection;

/// <summary>
/// Object3d登録管理クラス
/// </summary>
class Object3DRegistry : public RegistryBase<Object3DRegistry, Object3d> {
public:
    Object3DRegistry()  = default;
    ~Object3DRegistry() = default;

    /// <summary>
    /// オブジェクト登録
    /// </summary>
    void RegisterObject(Object3d* object);

    // 全描画、更新
    void UpdateAll();
    void DrawAll(const ViewProjection& viewProjection);
    void DrawAllShadow(const ViewProjection& viewProjection);

    // ImGuiデバッグ表示
    void DebugImGui(); 
};

}; // KetaEngine
