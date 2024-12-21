#pragma once


#include"BaseObject/BaseObject.h"


class BasePlayerHand:public BaseObject {
private:
	
private:

	///===========================================
	/// private variant
	///=============================================

	
public:
	
	///===========================================
	/// public method
	///=============================================

	virtual void Init();
	virtual void Update();
	virtual void Draw(const ViewProjection& viewprojection);
};