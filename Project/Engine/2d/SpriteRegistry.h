#pragma once

// std
#include <string>
#include <unordered_map>
#include <unordered_set>

// 前方宣言
namespace KetaEngine {

class Sprite;

/// <summary>
/// Spriteの登録管理クラス
/// </summary>
class SpriteRegistry {
private:
    SpriteRegistry() = default;
    ~SpriteRegistry();
    // コピー禁止
    SpriteRegistry(const SpriteRegistry&)            = delete;
    SpriteRegistry& operator=(const SpriteRegistry&) = delete;

public:
    static SpriteRegistry* GetInstance();

    /// ============================================================
    /// public methods
    /// ============================================================

    /// オブジェクト登録、解除
    void RegisterObject(Sprite* object);
    void UnregisterObject(Sprite* object);

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

    // スプライトの登録管理
    std::unordered_set<Sprite*> sprites_;
    std::unordered_map<std::string, Sprite*> groupRepresentatives_;

    // シングルトン管理
    static bool isDestroyed_;
    static SpriteRegistry* instance_;

public:
    bool HasGroupName(const std::string& groupName) const;
    const size_t& GetRegisteredCount() const { return sprites_.size(); }
};

}; // KetaEngine
