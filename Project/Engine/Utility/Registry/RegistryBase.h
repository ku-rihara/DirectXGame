#pragma once
#include <cstddef>
#include <unordered_set>

namespace KetaEngine {

/// <summary>
/// Object3DRegistryなどのRegistry系の基底クラス
/// </summary>
template <typename Derived, typename T>
class RegistryBase {
public:
    static Derived* GetInstance();

    void RegisterObject(T* object);
    void UnregisterObject(T* object);

    void Clear();
    size_t GetRegisteredCount() const;

protected:
    RegistryBase()  = default;
    ~RegistryBase() = default;

    // コピー禁止
    RegistryBase(const RegistryBase&)            = delete;
    RegistryBase& operator=(const RegistryBase&) = delete;


    //objects_をコピーし、走査中に解除された要素を除外しながらfuncを呼ぶ
    //Funcは呼び出し側ごとに型が変わるためヘッダ定義のまま
    template <typename Func>
    void ForEachAlive(Func&& func) {
        auto objectsCopy = objects_;
        for (T* obj : objectsCopy) {
            if (obj != nullptr && objects_.find(obj) != objects_.end()) {
                func(obj);
            }
        }
    }

protected:
    // 登録されているオブジェクトの集合
    std::unordered_set<T*> objects_;
};

} // namespace KetaEngine
