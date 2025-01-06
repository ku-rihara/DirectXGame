
#include "EnemyManager.h"
#include"NormalEnemy.h"
#include"StrongEnemy.h"

#include "Frame/Frame.h"

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
	currentPhase_ = 0;
	currentTime_ = 0.0f;

	// デフォルトのフェーズ0を作成
	if (phases_.find(0) == phases_.end()) {
		Phase phase;
		phases_[0] = phase;
	}
}


///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position) {
	
		std::unique_ptr<BaseEnemy> enemy;

		if (enemyType == "NormalEnemy") {// 通常敵
			enemy = std::make_unique<NormalEnemy>();		
		}
		if (enemyType == "StrongEnemy") {// 通常敵
			enemy = std::make_unique<StrongEnemy>();
		}

		// 位置初期化とlistに追加
		enemy->Init(position);
		enemy->SetPlayer(pPlayer_);// プレイヤーセット
		enemies_.push_back(std::move(enemy));
}


///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {
	if (isEditorMode_) {
		return; // エディタモード中は停止
	}

	SpawnUpdate(); // スポーン更新

	for (auto it = enemies_.begin(); it != enemies_.end(); ) {
		(*it)->Update(); // 更新

		if ((*it)->GetIsDeath()) {
			it = enemies_.erase(it); // 削除して次の要素を指すイテレータを取得
		}
		else {
			++it; // 削除しない場合はイテレータを進める
		}
	}

}

void EnemyManager::HpBarUpdate(const ViewProjection& viewProjection) {
	if (isEditorMode_) {
		return; // エディタモード中は停止
	}

	for (auto it = enemies_.begin(); it != enemies_.end();) {
		(*it)->DisplayHpBar(viewProjection);// 更新
		++it;
	}
}

void EnemyManager::SpawnUpdate() {
	currentTime_ += Frame::DeltaTimeRate(); //* 現在時間加算

	auto& phase = phases_[currentPhase_];//* 現在フェーズの取得

	// Waveの処理
	for (Wave& wave : phase.waves) {
		// Waveがまだ開始されていない場合はスキップ
		if (currentTime_ < wave.startTime) continue;

		for (auto& group : wave.groups) {
			// 次の生成時間に達していない場合はスキップ
			if (currentTime_ < group.spownTime) continue;
			if (group.isSpowned) continue;


			// グループ生成処理
			for (const auto& spawn : group.spownEnemies) {
				SpawnEnemy(spawn.enemyType, spawn.position);

			}
			group.isSpowned = true;
		}
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

///========================================================================================
///  ImGuiによるエディタ
///========================================================================================
void EnemyManager::ImGuiUpdate() {
	ImGui::Begin("Enemy Manager");

	// エディターモードフラグ
	ImGui::SeparatorText("EditorMode");
	ImGui::Checkbox("IsEditorMode", &isEditorMode_);

	//* フェーズ選択と Wave 編集
	ImGui::InputInt("Current Phase", &currentPhase_);
	auto& phase = phases_[currentPhase_];

	for (size_t waveIndex = 0; waveIndex < phase.waves.size(); ++waveIndex) {
		auto& wave = phase.waves[waveIndex];
		if (ImGui::CollapsingHeader(std::format("Wave {}", waveIndex).c_str())) {
			ImGui::DragFloat("Start Time", &wave.startTime, 0.1f);

			///--------------------------------------------------------
			/// グループ追加・削除
			///--------------------------------------------------------

			// Add Group: 青色
			
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));  // 青色に変更
			if (ImGui::Button("Add Group")) {
				wave.groups.push_back({ {}, 2.0f });
			}
			ImGui::PopStyleColor();

			
			// 敵の生成パラメータについて
			for (size_t groupIndex = 0; groupIndex < wave.groups.size(); ++groupIndex) {
				auto& spawn = wave.groups[groupIndex];
				ImGui::PushID(int(groupIndex));

				/// 敵のスポーンパターン(Typeと位置)
				if (ImGui::CollapsingHeader(("Group:" + std::to_string(groupIndex)).c_str())) {

					// Remove Group: 赤色
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));  // 赤色に変更
					if (ImGui::Button("Remove Group")) {
						wave.groups.erase(wave.groups.begin() + groupIndex);
						ImGui::PopID();
						ImGui::PopStyleColor();
						break;
					}
					ImGui::PopStyleColor();

					for (size_t spownEnemyIndex = 0; spownEnemyIndex < spawn.spownEnemies.size(); ++spownEnemyIndex) {
						auto& group = spawn.spownEnemies[spownEnemyIndex];
						ImGui::PushID(int(spownEnemyIndex));

						ImGui::Text("Enemy: %zu", spownEnemyIndex); // 敵のIndex表示
						ImGui::SameLine();

						// Remove Enemy: 赤色
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));  // 赤色に変更
						if (ImGui::Button("Remove Enemy")) {
							spawn.spownEnemies.erase(spawn.spownEnemies.begin() + spownEnemyIndex);
							ImGui::PopID(); // 敵のIDを解除
							ImGui::PopStyleColor();
							continue;
						}
						ImGui::PopStyleColor();

						// 敵グループの設定UI
						ImGui::InputText("Type", &group.enemyType[0], group.enemyType.size());
						ImGui::DragFloat3("Position", &group.position.x, 0.1f);

						ImGui::PopID(); // 敵ID解除
					}

					///-----------------------------------------------------------------------------------------
					/// 敵追加
					///-----------------------------------------------------------------------------------------

					// Add Enemy: 青色
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));  // 青色に変更
					static int selectedEnemyTypeIndex = 0; // 選択された敵タイプのインデックス

					// 敵の種類を選択する Combo
					if (ImGui::Combo("Select Enemy Type", &selectedEnemyTypeIndex, [](void* data, int idx, const char** out_text) {
						const auto& types = *reinterpret_cast<std::vector<std::string>*>(data);
						if (idx < 0 || idx >= static_cast<int>(types.size())) return false;
						*out_text = types[idx].c_str();
						return true;
						}, &enemyTypes_, static_cast<int>(enemyTypes_.size()))) {
						// Comboの選択が変更された場合の処理（必要に応じて追加）
					}

					// 敵を追加するボタン
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));  // 青色に変更
					if (ImGui::Button("Add Enemy")) {
						// 選択された敵タイプを基に敵を追加
						if (selectedEnemyTypeIndex >= 0 && selectedEnemyTypeIndex < static_cast<int>(enemyTypes_.size())) {
							spawn.spownEnemies.push_back({ enemyTypes_[selectedEnemyTypeIndex], {}, });
						}
					}
					ImGui::PopStyleColor();

					ImGui::PopStyleColor();

					// 生成するまでかかる時間
					ImGui::DragFloat("Spawn Time", &spawn.spownTime, 0.1f);
				}

				ImGui::PopID();
			}
		}
		///-----------------------------------------------------------------------------------------
		/// Wave
		///-----------------------------------------------------------------------------------------

		ImGui::SeparatorText("Wave");
																							
		// Remove Wave: 赤色
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));  // 赤色に変更
		if (ImGui::Button(std::format("Remove Wave ##{}", waveIndex).c_str())) {
			phase.waves.erase(phase.waves.begin() + waveIndex);
			ImGui::PopStyleColor();
		}
		ImGui::PopStyleColor();
	}

	/// Wave追加
	// Add Wave: 青色
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));  // 青色に変更
	if (ImGui::Button("Add Wave")) {
		phase.waves.push_back({ currentTime_, {} });
	}
	ImGui::PopStyleColor();

	/// セーブロード
	ImGui::SeparatorText("Save and Load");
	SaveAndLoad();

	ImGui::End();
}



///========================================================================================
/// フェーズ変更
///========================================================================================
void EnemyManager::SetPhase(int phase) {
	if (phases_.find(phase) != phases_.end()) {
		currentPhase_ = phase;
		currentTime_ = 0.0f;
	}
	else {
		ImGui::Text("Invalid phase!");
	}
}

///========================================================================================
/// エディターモード変更
///========================================================================================
void EnemyManager::SetEditorMode(bool isEditorMode) {
	isEditorMode_ = isEditorMode;
}


///========================================================================================
///  セーブ/ロード機能
///========================================================================================
void EnemyManager::SaveAndLoad() {
	if (ImGui::Button("Save Schedules")) {
		SaveEnemyPoPData();
	}

	if (ImGui::Button("Load Schedules")) {
		LoadEnemyPoPData();
	}
}

///========================================================================================
///  JSON を保存する関数
///========================================================================================
void EnemyManager::SaveEnemyPoPData() {
	json scheduleJson;

	scheduleJson["isEditorMode"] = isEditorMode_;// isEditorMode を保存

	// フェーズごとのデータをJSONに変換
	for (const auto& [phaseNum, phase] : phases_) {
		json phaseJson;
		for (const auto& wave : phase.waves) {
			json waveJson;
			waveJson["startTime"] = wave.startTime;
		
			// グループ
			for (const auto& group : wave.groups) {
				json groupJson;
				groupJson["spawnTime"] = group.spownTime;

				/// スポーンエネミー
				for (const auto& enemy : group.spownEnemies) {
					groupJson["spownEnemies"].push_back({
						{"enemyType", enemy.enemyType},
						{"position", {{"x", enemy.position.x}, {"y", enemy.position.y}, {"z", enemy.position.z}}}
						});
				}

				waveJson["spawns"].push_back(groupJson);
			}
			phaseJson["waves"].push_back(waveJson);
		}
		scheduleJson[std::to_string(phaseNum)] = phaseJson;
	}

	// ファイルに書き出し
	std::ofstream file(directrypath_ + filename_);
	if (file.is_open()) {
		file << scheduleJson.dump(4); // JSONデータを整形して書き出し
	}
}

///========================================================================================
///  JSON をロードする関数
///========================================================================================
void EnemyManager::LoadEnemyPoPData() {
	std::ifstream file(directrypath_ + filename_);
	if (file.is_open()) {
		json scheduleJson;
		file >> scheduleJson;

		// isEditorMode をロード
		if (scheduleJson.contains("isEditorMode")) {
			isEditorMode_ = scheduleJson["isEditorMode"].get<bool>();
		}

		phases_.clear(); // データをクリア

		for (const auto& [phaseNum, phaseData] : scheduleJson.items()) {
			// "isEditorMode" をスキップ
			if (phaseNum == "isEditorMode") continue;

			Phase phase;
			LoadPhase(phase, phaseData); // フェーズデータを読み込み
			phases_[std::stoi(phaseNum)] = phase;
		}
	}
}

///========================================================================================
///  フェーズデータをロードする補助関数
///========================================================================================
void EnemyManager::LoadPhase(Phase& phase, const json& phaseData) {
	if (!phaseData.contains("waves")) return;

	for (const auto& waveData : phaseData["waves"]) {
		Wave wave;

		// "startTime" の存在チェック
		wave.startTime = waveData.contains("startTime") ? waveData["startTime"].get<float>() : 0.0f;

		if (waveData.contains("spawns")) {
			for (const auto& spawnData : waveData["spawns"]) {
				EnemyGroup group;
				group.isSpowned = false;
				LoadSpawn(group, spawnData); // スポーンデータを読み込み
				wave.groups.push_back(group);
			}
		}

		phase.waves.push_back(wave);
	}
}

///========================================================================================
///  スポーンデータをロードする補助関数
///========================================================================================
void EnemyManager::LoadSpawn(EnemyGroup& group, const json& spawnData) {
	// "spawnTime" の存在チェック
	group.spownTime = spawnData.contains("spawnTime") ? spawnData["spawnTime"].get<float>() : 0.0f;

	// "spownEnemies" の存在チェック
	if (spawnData.contains("spownEnemies")) {
		for (const auto& groupData : spawnData["spownEnemies"]) {
			if (groupData.contains("enemyType") && groupData.contains("position")) {
				const auto& positionData = groupData["position"];

				// 各軸の存在チェック
				float x = positionData.contains("x") ? positionData["x"].get<float>() : 0.0f;
				float y = positionData.contains("y") ? positionData["y"].get<float>() : 0.0f;
				float z = positionData.contains("z") ? positionData["z"].get<float>() : 0.0f;

				group.spownEnemies.push_back({
					groupData["enemyType"].get<std::string>(),
					{x, y, z}
					});
			}
		}
	}
}


void EnemyManager::SetPlayer(Player* player) {
	pPlayer_ = player;
}