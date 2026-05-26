#pragma once
#include <memory>
#include <vector>

template <typename T>
class ObjectPool {
public:
    /// <summary>
    /// オブジェクトプールを事前に指定した数だけ確保する
    /// </summary>
    /// <param name="capacity">確保するオブジェクトの数</param>
    void PreAllocate(int capacity);

    // オブジェクトをプールから取得する
    std::unique_ptr<T> Acquire();

    /// <summary>
    /// // オブジェクトをプールに返却する
    /// </summary>
    /// <param name="obj">返却するオブジェクト</param>
    void Release(std::unique_ptr<T> obj);

private:
    // プールされたオブジェクトのコンテナ
    std::vector<std::unique_ptr<T>> pool_;

public:
    // 空かどうか
    bool IsEmpty() const;
    // プール内のオブジェクト数
    size_t Size() const;
};
