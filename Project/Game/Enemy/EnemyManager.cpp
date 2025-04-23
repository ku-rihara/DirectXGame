
#include "EnemyManager.h"
#include"NormalEnemy.h"
#include"StrongEnemy.h"
#include"base/TextureManager.h"

#include "Frame/Frame.h"
#include"LockOn/LockOn.h"

#include <format>
#include <fstream>
#include <imgui.h>

EnemyManager::EnemyManager() {

}

///========================================================================================
///  初期化
///========================================================================================

void EnemyManager::Init() {
	selectedEnemyType_ = "NormalEnemy";
	spownPosition_ = {};

  
	spownNum_ = 1;
	

	///* グローバルパラメータ
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(groupName_, false);
	AddParmGroup();
	ApplyGlobalParameter();

	ParticleInit(); /// パーティクル初期化

}

void  EnemyManager::FSpawn() {
	SpawnEnemy("NormalEnemy", Vector3{-30,0,40});
	SpawnEnemy("StrongEnemy", Vector3{ 30,0,40 });
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position) {
	
		std::unique_ptr<BaseEnemy> enemy;

		if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) {// 通常敵
			enemy = std::make_unique<NormalEnemy>();
			enemy->SetParamater(BaseEnemy::Type::NORMAL,paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
		}
		if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)]) {// 通常敵
			enemy = std::make_unique<StrongEnemy>();
			enemy->SetParamater(BaseEnemy::Type::STRONG,paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
		}

		// 位置初期化とlistに追加
		enemy->Init(position);
		enemy->SetPlayer(pPlayer_);// プレイヤーセット
		enemy->SetGameCamera(pGameCamera_);
		enemy->SetManager(this);
		enemies_.push_back(std::move(enemy));
}


///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {
	

	//SpawnUpdate(); // スポーン更新

	for (auto it = enemies_.begin(); it != enemies_.end(); ) {

		if ((*it)->GetType() == BaseEnemy::Type::NORMAL) {
			(*it)->SetParamater(BaseEnemy::Type::NORMAL, paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
		}
		else if ((*it)->GetType() == BaseEnemy::Type::STRONG) {
			(*it)->SetParamater(BaseEnemy::Type::STRONG, paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
		}

		(*it)->Update(); // 更新

		if ((*it)->GetIsDeath()) {
			pLockOn_->OnEnemyDestroyed((*it).get());
			it = enemies_.erase(it); // 削除して次の要素を指すイテレータを取得
			GetIsEnemiesCleared(); // フラグを更新
		}
		else {
			++it; // 削除しない場合はイテレータを進める
		}
	}
	ParticleUpdate(); // パーティクル更新
}

void EnemyManager::HpBarUpdate(const ViewProjection& viewProjection) {
	//if (isEditorMode_) {
	//	return; // エディタモード中は停止
	//}

	for (auto it = enemies_.begin(); it != enemies_.end();) {
		(*it)->DisplaySprite(viewProjection);// 更新
		++it;
	}
}




///========================================================================================
///  描画処理
///========================================================================================
void EnemyManager::Draw(const ViewProjection& viewProjection) {
	for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
		(*it)->Draw(viewProjection);
	}
}

///========================================================================================
///  スプライト描画処理
///========================================================================================
void EnemyManager::SpriteDraw(const ViewProjection& viewProjection) {
	for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
		(*it)->SpriteDraw(viewProjection);
	}
}

void EnemyManager::SetPlayer(Player* player) {
	pPlayer_ = player;
}   
void EnemyManager::SetLockon(LockOn* lockOn) {
	pLockOn_ = lockOn;
}

void EnemyManager::GetIsEnemiesCleared() {
	areAllEnemiesCleared_ = enemies_.empty(); // 現在の敵リストが空かを確認

	
}

void EnemyManager::SetGameCamera(GameCamera* gamecamera) {
	pGameCamera_ = gamecamera;
}


///=================================================================================
/// ロード
///=================================================================================
void EnemyManager::ParmLoadForImGui() {

	/// ロードボタン
	if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

		globalParameter_->LoadFile(groupName_);
		/// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", groupName_.c_str());
		ApplyGlobalParameter();
	}
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void EnemyManager::AddParmGroup() {

	for (uint32_t i = 0; i < paramaters_.size(); ++i) {
		globalParameter_->AddItem(groupName_,"chaseDistance" + std::to_string(int(i + 1)),paramaters_[i].chaseDistance);
		globalParameter_->AddItem(groupName_,"chaseSpeed" + std::to_string(int(i + 1)),paramaters_[i].chaseSpeed);
		globalParameter_->AddItem(groupName_, "basePosY_" + std::to_string(int(i + 1)), paramaters_[i].basePosY);
		globalParameter_->AddItem(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].thrustRotateSpeed);
		globalParameter_->AddItem(groupName_, "upperGravity" + std::to_string(int(i + 1)), paramaters_[i].upperGravity);
		globalParameter_->AddItem(groupName_, "upperJumpPower" + std::to_string(int(i + 1)), paramaters_[i].upperJumpPower);
		globalParameter_->AddItem(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)), paramaters_[i].upperFallSpeedLimit);
		globalParameter_->AddItem(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)), paramaters_[i].archingbackTime);
		globalParameter_->AddItem(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)), paramaters_[i].archingbackValue);
		globalParameter_->AddItem(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)), paramaters_[i].archingbackRatio);
		globalParameter_->AddItem(groupName_, "blowValue_" + std::to_string(int(i + 1)), paramaters_[i].blowValue);
		globalParameter_->AddItem(groupName_, "blowValueY_" + std::to_string(int(i + 1)), paramaters_[i].blowValueY);
		globalParameter_->AddItem(groupName_, "blowTime" + std::to_string(int(i + 1)), paramaters_[i].blowTime);
		globalParameter_->AddItem(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].blowRotateSpeed);
		globalParameter_->AddItem(groupName_, "burstTime" + std::to_string(int(i + 1)), paramaters_[i].burstTime);
		globalParameter_->AddItem(groupName_, "blowGravity" + std::to_string(int(i + 1)), paramaters_[i].blowGravity);

	}

}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void EnemyManager::SetValues() {


	for (uint32_t i = 0; i < paramaters_.size(); ++i) {
		globalParameter_->SetValue(groupName_,"chaseDistance" + std::to_string(int(i + 1)),paramaters_[i].chaseDistance);
		globalParameter_->SetValue(groupName_,"chaseSpeed" + std::to_string(int(i + 1)),paramaters_[i].chaseSpeed);
		globalParameter_->SetValue(groupName_, "basePosY_" + std::to_string(int(i + 1)), paramaters_[i].basePosY);
		globalParameter_->SetValue(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].thrustRotateSpeed);
		globalParameter_->SetValue(groupName_, "upperGravity" + std::to_string(int(i + 1)), paramaters_[i].upperGravity);
		globalParameter_->SetValue(groupName_, "upperJumpPower" + std::to_string(int(i + 1)), paramaters_[i].upperJumpPower);
		globalParameter_->SetValue(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)), paramaters_[i].upperFallSpeedLimit);
		globalParameter_->SetValue(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)), paramaters_[i].archingbackTime);
		globalParameter_->SetValue(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)), paramaters_[i].archingbackValue);
		globalParameter_->SetValue(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)), paramaters_[i].archingbackRatio);
		globalParameter_->SetValue(groupName_, "blowValue_" + std::to_string(int(i + 1)), paramaters_[i].blowValue);
		globalParameter_->SetValue(groupName_, "blowValueY_" + std::to_string(int(i + 1)), paramaters_[i].blowValueY);
		globalParameter_->SetValue(groupName_, "blowTime" + std::to_string(int(i + 1)), paramaters_[i].blowTime);
		globalParameter_->SetValue(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].blowRotateSpeed);
		globalParameter_->SetValue(groupName_, "burstTime" + std::to_string(int(i + 1)), paramaters_[i].burstTime);
		globalParameter_->SetValue(groupName_, "blowGravity" + std::to_string(int(i + 1)), paramaters_[i].blowGravity);

	}

}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void EnemyManager::ApplyGlobalParameter() {
	/// パラメータ
	for (uint32_t i = 0; i < paramaters_.size(); ++i) {
		paramaters_[i].chaseDistance = globalParameter_->GetValue<float>(groupName_,"chaseDistance" + std::to_string(int(i + 1)));
		paramaters_[i].chaseSpeed = globalParameter_->GetValue<float>(groupName_,"chaseSpeed" + std::to_string(int(i + 1)));
		paramaters_[i].basePosY = globalParameter_->GetValue<float>(groupName_, "basePosY_" + std::to_string(int(i + 1)));
		paramaters_[i].thrustRotateSpeed = globalParameter_->GetValue<float>(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)));
		paramaters_[i].upperGravity = globalParameter_->GetValue<float>(groupName_, "upperGravity" + std::to_string(int(i + 1)));
		paramaters_[i].upperJumpPower = globalParameter_->GetValue<float>(groupName_, "upperJumpPower" + std::to_string(int(i + 1)));
		paramaters_[i].upperFallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)));
		paramaters_[i].archingbackValue = globalParameter_->GetValue<float>(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)));
		paramaters_[i].archingbackRatio = globalParameter_->GetValue<float>(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)));
		paramaters_[i].archingbackTime = globalParameter_->GetValue<float>(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)));
		paramaters_[i].blowValue = globalParameter_->GetValue<float>(groupName_, "blowValue_" + std::to_string(int(i + 1)));
		paramaters_[i].blowValueY = globalParameter_->GetValue<float>(groupName_, "blowValueY_" + std::to_string(int(i + 1)));
		paramaters_[i].blowTime = globalParameter_->GetValue<float>(groupName_, "blowTime" + std::to_string(int(i + 1)));
		paramaters_[i].blowRotateSpeed = globalParameter_->GetValue<float>(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)));
		paramaters_[i].blowGravity = globalParameter_->GetValue<float>(groupName_, "blowGravity" + std::to_string(int(i + 1)));
		paramaters_[i].burstTime = globalParameter_->GetValue<float>(groupName_, "burstTime" + std::to_string(int(i + 1)));


	}

}

///=========================================================
/// パラメータ調整
///==========================================================
void EnemyManager::AdjustParm() {
	SetValues();
#ifdef _DEBUG

	if (ImGui::CollapsingHeader(groupName_.c_str())) {
		ImGui::PushID(groupName_.c_str());
		///---------------------------------------------------------
		/// 通常敵
		///----------------------------------------------------------

		ImGui::SeparatorText(enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].c_str());
		ImGui::PushID(enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].c_str());

		ImGui::DragFloat("ChaseSpeed",&paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].chaseSpeed,
			0.01f);

		ImGui::DragFloat("ChaseDistance",&paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].chaseDistance,
			0.01f);

		ImGui::DragFloat("basePosY", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].basePosY,
			0.01f);

		ImGui::DragFloat("thrustRotateSpeed", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].thrustRotateSpeed,
			0.01f);

		ImGui::DragFloat("upperGravity", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].upperGravity,
			0.01f);

		ImGui::DragFloat("upperJumpPowe", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].upperJumpPower,
			0.01f);

		ImGui::DragFloat("upperFallSpeedLimit", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].upperFallSpeedLimit,
			0.01f);

		ImGui::SliderAngle("hitbackRotateValue", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].archingbackValue, -360.0f, 720.0f
		);

		ImGui::DragFloat("hitbackRotateBackRatio", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].archingbackRatio,
			0.01f);

		ImGui::DragFloat("hitbackRotateTime", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].archingbackTime,
			0.01f);

		ImGui::DragFloat("blowValue", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].blowValue,
			0.01f);

		ImGui::DragFloat("blowValueY", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].blowValueY,
			0.01f);

		ImGui::DragFloat("blowRotateSpeed", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].blowRotateSpeed,
			0.01f);

		ImGui::DragFloat("burstTime", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].burstTime,
			0.01f);

		ImGui::DragFloat("blowGravity", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].blowGravity,
			0.01f);
		
		ImGui::DragFloat("blowTime", &paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)].blowTime,
			0.01f);
		ImGui::PopID();

		///---------------------------------------------------------
		/// ストロングな敵
		///----------------------------------------------------------

		ImGui::SeparatorText(enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)].c_str());
		ImGui::PushID(enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)].c_str());

		ImGui::DragFloat("ChaseSpeed",&paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].chaseSpeed,
			0.01f);

		ImGui::DragFloat("ChaseDistance",&paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].chaseDistance,
			0.01f);

		ImGui::DragFloat("basePosY", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].basePosY,
			0.01f);

		ImGui::DragFloat("thrustRotateSpeed", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].thrustRotateSpeed,
			0.01f);

		ImGui::DragFloat("upperGravity", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].upperGravity,
			0.01f);

		ImGui::DragFloat("upperJumpPowe", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].upperJumpPower,
			0.01f);

		ImGui::DragFloat("upperFallSpeedLimit", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].upperFallSpeedLimit,
			0.01f);

		ImGui::SliderAngle("hitbackRotateValue", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].archingbackValue,-360.0f,720.0f
		);

		ImGui::DragFloat("hitbackRotateBackRatio", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].archingbackRatio,
			0.01f);

		ImGui::DragFloat("hitbackRotateTime", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].archingbackTime,
			0.01f);

		ImGui::DragFloat("blowValue", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].blowValue,
			0.01f);

		ImGui::DragFloat("blowValueY", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].blowValueY,
			0.01f);

		ImGui::DragFloat("blowRotateSpeed", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].blowRotateSpeed,
			0.01f);

		ImGui::DragFloat("burstTime", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].burstTime,
			0.01f);

		ImGui::DragFloat("blowGravity", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].blowGravity,
			0.01f);

		ImGui::DragFloat("blowTime", &paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)].blowTime,
			0.01f);

		ImGui::PopID();
		/// セーブとロード
		globalParameter_->ParmSaveForImGui(groupName_);
		ParmLoadForImGui();

		ImGui::PopID();
	}

#endif // _DEBUG
}

void EnemyManager::ParticleInit() {
	uint32_t circleHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/circle.png");
	uint32_t defaultHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/default.png");
	uint32_t boalHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/boal.png");
	uint32_t crackTexture_ = TextureManager::GetInstance()->LoadTexture("Resources/Texture/Crack.png");

	//damage
    damageEffect[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffectCenter", PrimitiveType::Plane, 100));
    damageEffect[1].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffect", PrimitiveType::Plane, 100));
    damageEffect[2].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitBlow", PrimitiveType::Plane, 100));
    damageEffect[3].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffectStar", PrimitiveType::Plane, 100));

	/// death
	InitParticleEffect(deathParticle_[0], "EnemyDeathSmoke", "Plane", boalHandle, 900);
	deathParticle_[0].emitter->SetBlendMode(BlendMode::None);
	InitParticleEffect(deathParticle_[1], "EnemyDeathFireSmoke", "Plane", circleHandle, 900);
	InitParticleEffect(deathParticle_[2], "EnemyDeathSpark", "Plane", circleHandle, 900);
	InitParticleEffect(deathParticle_[3], "EnemyDeathMiniSpark", "Plane", circleHandle, 900);

	//
	InitParticleEffect(debriParticle_[0], "DebriName", "debri", defaultHandle, 500);
	debriParticle_[0].emitter->SetBlendMode(BlendMode::None);

	// crack
	fallCrack_.reset(ParticleEmitter::CreateParticle("Crack", "Plane", ".obj", 30));
	fallCrack_->SetTextureHandle(crackTexture_);
	fallCrack_->SetBlendMode(BlendMode::None);

	

}

///---------------------------------------------------------
/// Particle
///----------------------------------------------------------

void EnemyManager::InitParticleEffect(ParticleEffect& effect, const std::string& name, const std::string& modelName, const uint32_t& textureHandle, const int32_t& maxnum) {
	effect.name = name;
	effect.emitter.reset(ParticleEmitter::CreateParticle(name, modelName, ".obj", maxnum));
	effect.emitter->SetTextureHandle(textureHandle);
}

void EnemyManager::DamageEmit(const Vector3& pos) {
	for (uint32_t i = 0; i < damageEffect.size(); i++) {
		damageEffect[i].emitter->SetTargetPosition(pos);
		damageEffect[i].emitter->Emit();

	}
}

void EnemyManager::ThrustEmit(const Vector3& pos) {
	//ガレキパーティクル
	for (uint32_t i = 0; i < debriParticle_.size(); i++) {
		debriParticle_[i].emitter->SetTargetPosition(pos);
		debriParticle_[i].emitter->Emit();
	}
	fallCrack_->SetTargetPosition(Vector3(pos.x, 0.0f, pos.z));
	fallCrack_->Emit();
	
}

void EnemyManager::DeathEmit(const Vector3&pos) {
	// 死亡パーティクル
	for (uint32_t i = 0; i < deathParticle_.size(); i++) {
		deathParticle_[i].emitter->SetTargetPosition(pos);
		deathParticle_[i].emitter->Emit();
	}
}

void EnemyManager::ParticleUpdate() {
	// 死亡パーティクル
	for (uint32_t i = 0; i < damageEffect.size(); i++) {
	
		damageEffect[i].emitter->Update();
	}

	fallCrack_->Update();


	// 死亡パーティクル
	for (uint32_t i = 0; i < deathParticle_.size(); i++) {
		deathParticle_[i].emitter->Update();
	}

	//ガレキパーティクル
	for (uint32_t i = 0; i < debriParticle_.size(); i++) {
		
		debriParticle_[i].emitter->Update();
	}

}