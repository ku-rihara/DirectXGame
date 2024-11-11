#pragma once

#include"ControlPoint/BaseSpeedControl.h"

class NormalPoint:public BaseSpeedControl {
	static int instanceNum_;

private:

	///============================================
	///private variants
	/// ===========================================
	
	
	int instanceID_;
	
public://メンバ関数

	void Init()override;
	void Update()override;
	void Debug()override;
	void Draw(const ViewProjection& viewProjection)override;

	
};