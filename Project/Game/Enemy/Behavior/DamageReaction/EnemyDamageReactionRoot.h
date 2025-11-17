#pragma once

#include"BaseEnemyDamageReaction.h"


class EnemyDamageReactionRoot : public BaseEnemyDamageReaction {
private:

private:

	

public:
	//コンストラクタ
    EnemyDamageReactionRoot(BaseEnemy* boss);
    ~EnemyDamageReactionRoot();


	void Update()override;
	void Debug()override;

};
