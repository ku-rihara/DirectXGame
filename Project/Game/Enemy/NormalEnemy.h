#pragma once
#include <list>
#include <memory>
//Function
#include "Easing.h"
// class
#include "BaseEnemy.h"

/// <summary>
/// 敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
private:
	

private: 
	
public: 

	///========================================================================================
	///  public method
	///========================================================================================

	// 初期化、更新、描画
	void Init(const Vector3& spownPos) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;
	void SpriteDraw(const ViewProjection& viewProjection)override;

	/// Hpバー処理
	void DisplayHpBar(const ViewProjection& viewProjection)override;
	
	
};