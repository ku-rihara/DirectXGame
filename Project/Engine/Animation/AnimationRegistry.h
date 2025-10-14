#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

// 前方宣言
class Object3DAnimation;
class ViewProjection;

class AnimationRegistry {
public:
    /// シングルトンインスタンス取得
    static AnimationRegistry* GetInstance();

    /// ============================================================
    /// public methods
    /// ============================================================

    /// 登録、解除
    void RegisterAnimation(Object3DAnimation* animation);
    void UnregisterAnimation(Object3DAnimation* animation);

    /// 更新
    void UpdateAll(const float& deltaTime);

    /// 描画
    void DrawAll(const ViewProjection& viewProjection);
    void DrawAllShadow(const ViewProjection& viewProjection);
    void DebugDrawAll(const ViewProjection& viewProjection);

    void Clear();
    void DebugImGui();

private:
    AnimationRegistry()  = default;
    ~AnimationRegistry();

    /// コピー禁止
    AnimationRegistry(const AnimationRegistry&)            = delete;
    AnimationRegistry& operator=(const AnimationRegistry&) = delete;

private:
    std::unordered_set<Object3DAnimation*> animations_;
    static AnimationRegistry* instance_;
    static bool isDestroyed_;

public:
    const size_t& GetRegisteredCount() const { return animations_.size(); }
};