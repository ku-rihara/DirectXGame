#pragma once
#include "2d/Sprite.h"
#include"3d/WorldTransform.h"
#include "3d/ViewProjection.h"
// std
#include <list>
#include<memory>
class Player;
//class GameScene;
class Reticle {
private:
	//包含
	Player* pPlayer_;
	std::unique_ptr<Sprite>sprite_ = nullptr;
	Vector2 Reticle2DPos_;
	Vector2 rockPos_;
	float reticleMoveTime_;
	WorldTransform worldTransform3DReticle_;
	uint32_t textureHandle_;

public:
	Reticle();
	~Reticle();
	void Init();
	void Updata(const ViewProjection& viewProjection);
	void Draw();


	/// プレイヤーセット
	void SetPlayer(Player* player);

	Vector3 GetWorld3DRecticlPos();
	Vector2 Get2DReticlePos() const { return Reticle2DPos_; }



};
