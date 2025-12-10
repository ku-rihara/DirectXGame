#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

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

    /// コピー禁止
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

    void UpdateAll(); //<全スプライト更新
    void DrawAll();   //<全スプライト描画

    void Clear();      //<登録解除
    void DebugImGui(); //<ImGuiデバッグ

    /// ============================================================
    /// private members
    /// ============================================================
private:
    static bool isDestroyed_;

    std::unordered_set<Sprite*> sprites_;
    static SpriteRegistry* instance_;

public:
    bool HasGroupName(const std::string& groupName) const;
    const size_t& GetRegisteredCount() const { return sprites_.size(); }
};

}; // KetaEngine
