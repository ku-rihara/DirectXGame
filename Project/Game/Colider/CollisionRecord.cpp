#include "CollisionRecord.h"
#include <algorithm>

void CollisionRecord::AddHistory(uint32_t number) {
	// 履歴に登録
	history_.push_back(number);
}

bool CollisionRecord::CheckHistory(uint32_t number) {
	
	bool result = std::any_of(history_.begin(), history_.end(), [number](uint32_t n) { 
		return n == number; }
	);
	return result;
}

void CollisionRecord::Clear() { 
	history_.clear();
	}