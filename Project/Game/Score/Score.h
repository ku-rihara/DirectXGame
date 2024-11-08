#pragma once
#include"2d/Sprite.h"
/// std
#include<memory>
#include<vector>

class Score {
private:
	///============================================
	///private variants
	/// ===========================================
	std::vector<std::unique_ptr<Sprite>> numSprite_;
	std::unique_ptr<Sprite> scoreFrameSprite_;
	float size_;
	int score_;
public:

	///============================================
	///public method
	/// ===========================================
	
	void Init();
	void Update();
	void Draw();
	void Debug();

	void ScoreUp(int num);
};