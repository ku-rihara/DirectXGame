#pragma once

// std
#include <string>
#include <unordered_map>

#include "Utility/Registry/RegistryBase.h"

// 前方宣言
namespace KetaEngine {

class Sprite;

/// <summary>
/// Spriteの登録管理クラス
/// </summary>
class SpriteRegistry : public RegistryBase<SpriteRegistry, Sprite> {
public:
    SpriteRegistry()  = default;
    ~SpriteRegistry() = default;

    /// ============================================================
    /// public methods
    /// ============================================================

    // 全スプライト、更新、描画
    void UpdateAll();
    void DrawAll();

    void Clear();      //<登録解除
    void DebugImGui(); //<ImGuiデバッグ

    /// 代表スプライト管理
    void RegisterRepresentative(const std::string& groupName, Sprite* sprite);
    void UnregisterRepresentative(Sprite* sprite);
    Sprite* GetRepresentative(const std::string& groupName) const;

private:
    /// ============================================================
    /// private variable
    /// ============================================================

    std::unordered_map<std::string, Sprite*> groupRepresentatives_;

public:
    bool HasGroupName(const std::string& groupName) const;
};

}; // KetaEngine
