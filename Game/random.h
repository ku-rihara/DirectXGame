#pragma once
#include<random>
// ランダム数生成器の初期化
static std::random_device rd;  
static std::mt19937 random(rd()); 

