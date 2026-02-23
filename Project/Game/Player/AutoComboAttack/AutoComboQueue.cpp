#include "AutoComboQueue.h"

void AutoComboQueue::Enqueue(PlayerComboAttackData* data) {
    queue_.push(data);
}

PlayerComboAttackData* AutoComboQueue::Dequeue() {
    if (queue_.empty()) {
        return nullptr;
    }
    PlayerComboAttackData* front = queue_.front();
    queue_.pop();
    return front;
}

PlayerComboAttackData* AutoComboQueue::Peek() const {
    if (queue_.empty()) {
        return nullptr;
    }
    return queue_.front();
}

bool AutoComboQueue::IsEmpty() const {
    return queue_.empty();
}

void AutoComboQueue::Clear() {
    while (!queue_.empty()) {
        queue_.pop();
    }
}