#include "GlobalParameter.h"
#include "base/WinApp.h"
#include <fstream>
#include <imgui.h>

template void GlobalParameter::SetValue<int>(const std::string& groupName, const std::string& key, int value);
template void GlobalParameter::SetValue<float>(const std::string& groupName, const std::string& key, float value);
template void GlobalParameter::SetValue<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
template void GlobalParameter::SetValue<bool>(const std::string& groupName, const std::string& key, bool value);

template void GlobalParameter::SetValue2<int>(const std::string& groupName, const std::string& key, int value);
template void GlobalParameter::SetValue2<float>(const std::string& groupName, const std::string& key, float value);
template void GlobalParameter::SetValue2<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
template void GlobalParameter::SetValue2<bool>(const std::string& groupName, const std::string& key, bool value);


template void GlobalParameter::AddItem<int>(const std::string& groupName, const std::string& key, int value);
template void GlobalParameter::AddItem<float>(const std::string& groupName, const std::string& key, float value);
template void GlobalParameter::AddItem<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
template void GlobalParameter::AddItem<bool>(const std::string& groupName, const std::string& key, bool value);

template int32_t GlobalParameter::GetValue<int32_t>(const std::string& groupName, const std::string& key) const;
template float GlobalParameter::GetValue<float>(const std::string& groupName, const std::string& key) const;
template Vector3 GlobalParameter::GetValue<Vector3>(const std::string& groupName, const std::string& key) const;
template bool GlobalParameter::GetValue<bool>(const std::string& groupName, const std::string& key) const;

// シングルトンインスタンス
GlobalParameter* GlobalParameter::GetInstance() {
	static GlobalParameter instance;
	return &instance;
}

void GlobalParameter::CreateGroup(const std::string& groupName) {
	
	// 指定名のオブジェクトが無ければ追加する
	datas_[groupName];
}

void GlobalParameter::Update() {
#ifdef _DEBUG



	if (!ImGui::Begin("Global Parameter", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())
		return;

	// 各グループ
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end(); ++itGroup) {
		// グループ名を取得
		const std::string& groupName = itGroup->first;
		// グループ名の参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;
		for (std::map<std::string, Item>::iterator itItem = group.begin(); itItem != group.end(); ++itItem) {
			// 項目名を取得
			const std::string& itemName = itItem->first;
			// 項目の参照を取得
			Item& item = itItem->second;

			// int32_t型の値を肘していれば
			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}

			// float型の値を肘していれば
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.1f);
			}

			// Vector3型の値を肘していれば
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
			}

			// bool型の値を肘していれば
			else if (std::holds_alternative<bool>(item)) {
				bool* ptr = std::get_if<bool>(&item);
				ImGui::Checkbox(itemName.c_str(), reinterpret_cast<bool*>(ptr));
			}
		}
		// 改行
		ImGui::Text("\n");
		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
		}
		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End(); 
#endif // _DEBUG
}

template<typename T> void GlobalParameter::SetValue(const std::string& groupName, const std::string& key, T value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = value;
}

template<typename T> void GlobalParameter::AddItem(const std::string& groupName, const std::string& key, T value) {
	json root;
	// グループを検索
	json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	if (itGroup != root.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalParameter::SaveFile(const std::string& groupName) {
	// グル―プを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録をチェック
	assert(itGroup != datas_.end());

	json root;

	root = json::object();

	// jsonオブジェクト登録
	root[groupName] = json::object();

	// 各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin(); itItem != itGroup->second.end(); ++itItem) {
		// 項目名を取得
		const std::string& itemName = itItem->first;
		// 項目の参照を取得
		Item& item = itItem->second;

		// int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item)) {
			// int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		}

		// float型の値を保持していれば
		else if (std::holds_alternative<float>(item)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item);
		}

		// Vector3型の値を保持していれば
		else if (std::holds_alternative<Vector3>(item)) {
			// Vector3型の値を登録
			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = json::array({value.x, value.y, value.z});
		}
		// bool型の値を保持していれば
		else if (std::holds_alternative<bool>(item)) {
			// bool型の値を登録
			root[groupName][itemName] = std::get<bool>(item);
		}
	}
	// ディレクトリが無ければ作成する
	std::filesystem::path dri(kDirectoryPath);
	if (!std::filesystem::exists(dri)) {
		std::filesystem::create_directories(dri);
	}
	// 書き込むJSONファイルのフルパスを合成
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイル書き込み用に開く
	ofs.open(filePath);
	// ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalPatameter", 0);
		assert(0);
		return;
	}
	// ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	// ファイルを閉じる
	ofs.close();
}

void GlobalParameter::LoadFiles() {
	// 保存先のディレクトリパスをローカル変数で宣言する
	std::filesystem::path driPath(kDirectoryPath);
	// ディレクトリが無ければスキップ
	if (!std::filesystem::exists(driPath)) {
		return;
	}
	std::filesystem::directory_iterator dri_it(driPath);
	for (const std::filesystem::directory_entry& entry : dri_it) {
		// ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();
		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}
		// ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalParameter::LoadFile(const std::string& groupName) {

	// 読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗?
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalPatameter", 0);
		assert(0);
		return;
	}
	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();
	// グループを検索
	json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	assert(itGroup != root.end());

	// 各アイテムについて
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		// アイテム名を取得
		const std::string& itemName = itItem.key();

		// int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			// int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		// float型の値を保持していれば
		else if (itItem->is_number_float()) {
			// float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		// 要素数3の配列であれば
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列登録
			Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName, value);
		}
		// boolであれば
		else if (itItem->is_boolean()) {
			// bool型のjson配列登録
			bool value = itItem->get<bool>();
			SetValue(groupName, itemName, value);
		}
	}
}

template<typename T> T GlobalParameter::GetValue(const std::string& groupName, const std::string& key) const {

	// グル―プを検索
	std::map<std::string, Group>::const_iterator itGroup = datas_.find(groupName);
	// 指定グループが存在するか
	assert(itGroup != datas_.end());

	// グループの参照を取得
	const Group& group = datas_.at(groupName);

	// キーが存在するかを確認
	std::map<std::string, Item>::const_iterator itItem = group.find(key);
	assert(itItem != group.end());

	// キーに対応する値を返す
	return std::get<T>(itItem->second);
}

template<typename T>
void GlobalParameter::SetValue2(const std::string& groupName, const std::string& key, T value) {
	Group& group = datas_[groupName];

	// 既存の値と新しい値が異なる場合のみ、変更を記録する
	if (group.find(key) == group.end() || std::get<T>(group[key]) != value) {
		group[key] = value;
		isValueChanged_[groupName] = true; // 変更フラグを立てる
	}
}

void GlobalParameter::Update2() {
	for (const auto& [groupName, group] : datas_) {
		// 変更されたグループのみを保存
		if (isValueChanged_[groupName]) {

			SaveFile(groupName);
			isValueChanged_[groupName] = false; // 保存後、フラグをリセット
		}
	}
}