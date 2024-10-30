#pragma once
#include<vector>
class CollisionRecord {
private:
	//履歴
	std::vector<uint32_t> history_;

public:
	//履歴追加
	void AddHistory(uint32_t number);
	//履歴チェック
	bool CheckHistory(uint32_t number);
	//履歴抹消
	void Clear();
	
};
