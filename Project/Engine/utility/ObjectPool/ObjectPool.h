#pragma once
#include <cassert>
#include <memory>
#include <vector>

template <typename T>
class ObjectPool {
public:
    /// ゲーム開始時に capacity 個のオブジェクトを事前確保する
    void PreAllocate(int capacity) {
        pool_.reserve(pool_.size() + capacity);
        for (int i = 0; i < capacity; ++i) {
            pool_.push_back(std::make_unique<T>());
        }
    }

    // プールから取得
    std::unique_ptr<T> Acquire() {
        if (pool_.empty()) {
            return nullptr;
        }
        auto obj = std::move(pool_.back());
        pool_.pop_back();
        return obj;
    }

    // プールに返却
    void Release(std::unique_ptr<T> obj) {
        if (obj) {
            pool_.push_back(std::move(obj));
        }
    }

private:
    std::vector<std::unique_ptr<T>> pool_;

public:
    bool IsEmpty() const { return pool_.empty(); }
    size_t Size() const { return pool_.size(); }
};
