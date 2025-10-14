#pragma once

#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

// 前方宣言
class Sprite;

class SpriteRegistry {
public:
  
    static SpriteRegistry* GetInstance();

    /// ============================================================
    /// public methods
    /// ============================================================

    /// オブジェクト登録、解除
    void RegisterObject(Sprite* object);
    void UnregisterObject(Sprite* object);

    /// 更新、描画
    void UpdateAll();
    void DrawAll();

    void Clear();
    void DebugImGui();

private:
    SpriteRegistry()  = default;
    ~SpriteRegistry();

    /// コピー禁止
    SpriteRegistry(const SpriteRegistry&)            = delete;
    SpriteRegistry& operator=(const SpriteRegistry&) = delete;

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