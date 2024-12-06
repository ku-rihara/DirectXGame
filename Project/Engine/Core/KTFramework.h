#pragma once
#include<Scene/GameScene.h>

///utility
#include"utility/Editor/GlobalParameter.h"
#include "Frame/Frame.h"
#include"Collider/CollisionManager.h"
/// std
#include<memory>

// ゲーム全体
class KTFramework {
protected:
	
	std::unique_ptr<GameScene>gameScene_;

	std::unique_ptr<CollisionManager>collisionManager_;
	
public:

	virtual ~KTFramework() = default;

	///======================================
	///public method
	///======================================
	
	virtual void Init();
	virtual void Update();
	virtual void Draw() = 0;
	virtual void Finalize();

	void DisplayFPS();/// FPS表示



	void Run(); /// 実行

};