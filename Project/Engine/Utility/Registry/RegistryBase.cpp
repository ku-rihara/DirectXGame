#include "RegistryBase.h"
using namespace KetaEngine;

// Registry
#include "2D/SpriteRegistry.h"
#include "3D/AnimationObject3D/AnimationRegistry.h"
#include "3D/Object3D/Object3DRegistry.h"

#include <cstdlib>

///============================================================
/// シングルトンインスタンス取得
///============================================================
template <typename Derived, typename T>
Derived* RegistryBase<Derived, T>::GetInstance() {
    static Derived instance;
    static bool isAlive = true;

    // アプリケーション終了処理中かチェック
    if (!isAlive) {
        return nullptr;
    }

    // 初回呼び出し時にAtexit登録
    static bool registered = false;
    if (!registered) {
        std::atexit([]() { isAlive = false; });
        registered = true;
    }

    return &instance;
}

///============================================================
/// オブジェクト登録
///============================================================
template <typename Derived, typename T>
void RegistryBase<Derived, T>::RegisterObject(T* object) {
    if (object == nullptr) {
        return;
    }
    objects_.insert(object);
}

///============================================================
/// オブジェクト登録解除
///============================================================
template <typename Derived, typename T>
void RegistryBase<Derived, T>::UnregisterObject(T* object) {
    if (object == nullptr) {
        return;
    }
    objects_.erase(object);
}

///============================================================
/// 全オブジェクトクリア
///============================================================
template <typename Derived, typename T>
void RegistryBase<Derived, T>::Clear() {
    objects_.clear();
}

///============================================================
/// 登録数取得
///============================================================
template <typename Derived, typename T>
size_t RegistryBase<Derived, T>::GetRegisteredCount() const {
    return objects_.size();
}

///============================================================
/// 明示的インスタンス化
///============================================================
template class RegistryBase<Object3DRegistry, Object3d>;
template class RegistryBase<AnimationRegistry, Object3DAnimation>;
template class RegistryBase<SpriteRegistry, Sprite>;
