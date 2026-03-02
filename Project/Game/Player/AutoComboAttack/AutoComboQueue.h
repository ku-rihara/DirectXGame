#pragma once
#include <queue>

class PlayerComboAttackData;

/// <summary>
/// コンボ自動実行用FIFOキュー
/// </summary>
class AutoComboQueue {
public:
    /// キューに攻撃データを末尾へ追加
    void Enqueue(PlayerComboAttackData* data);
    /// 先頭の攻撃データを取り出す
    PlayerComboAttackData* Dequeue();
    /// 先頭の攻撃データを取り出さずに参照
    PlayerComboAttackData* Peek() const;
    /// キューが空かどうか
    bool IsEmpty() const;
    /// キューを全クリア
    void Clear();

private:
    std::queue<PlayerComboAttackData*> queue_;
};