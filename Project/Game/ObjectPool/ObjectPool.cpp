#include "ObjectPool.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/StrongEnemy.h"

template <typename T>
void ObjectPool<T>::PreAllocate(int capacity) {

    // 既存のオブジェクトをクリアしてから新たに確保する
    pool_.reserve(pool_.size() + capacity);

    // 事前に指定された数だけオブジェクトを生成してプールに追加
    for (int i = 0; i < capacity; ++i) {
        pool_.push_back(std::make_unique<T>());
    }
}

template <typename T>
std::unique_ptr<T> ObjectPool<T>::Acquire() {

    // プールが空の場合は nullptr を返す
    if (pool_.empty()) {
        return nullptr;
    }

    // 末尾から取り出す
    auto obj = std::move(pool_.back());
    pool_.pop_back();
    return obj;
}

template <typename T>
void ObjectPool<T>::Release(std::unique_ptr<T> obj) {

    // 返却されたオブジェクトをプールに追加
    if (obj) {
        pool_.push_back(std::move(obj));
    }
}

template <typename T>
bool ObjectPool<T>::IsEmpty() const {
    // プールが空かどうかを返す
    return pool_.empty();
}

template <typename T>
size_t ObjectPool<T>::Size() const {
    // プール内のオブジェクト数を返す
    return pool_.size();
}

// 明示的インスタンス化
template class ObjectPool<NormalEnemy>;
template class ObjectPool<StrongEnemy>;
