#pragma once
#include <memory>
#include <vector>

template <typename T>
class ObjectPool {
public:
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
