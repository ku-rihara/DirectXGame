#pragma once
#include "Vector3.h"

#include <map>
#include <string>
#include <variant>
#include<json.hpp>

class GlobalParameter {
private:
	//項目
	using Item =std::variant<int32_t, float, Vector3,bool>;
	//グループ
	using Group =std::map<std::string, Item>;
	// 全データ
	std::map<std::string, Group> datas_;
	//Json
	using json=nlohmann::json;

	std::map<std::string, bool> isValueChanged_;
	
	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalParameter/";

public:
	// シングルトン
	static GlobalParameter* GetInstance();
	void Update();
	void CreateGroup(const std::string& groupName);

	/// <summary>
	/// 値のセット
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="グループ名"></param>
	/// <param name="key"></param>
	/// <param name="値"></param>
	template<typename T> void SetValue(const std::string& groupName, const std::string& key, T value);
	/// <summary>
	/// 項目の追加
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="groupName"></param>
	/// <param name="key"></param>
	/// <param name="value"></param>
	template<typename T> void AddItem(const std::string& groupName, const std::string& key, T value);


	/// <summary>
	/// ファイルに書き出し
	/// </summary>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="グループ名"></param>
	void LoadFile(const std::string& groupName);

 template<typename T>T GetValue(const std::string& groupName, const std::string& key) const;


 template<typename T> void SetValue2(const std::string& groupName, const std::string& key, T value);
 void Update2();

private:
	GlobalParameter() = default;
	~GlobalParameter() = default;
	GlobalParameter(const GlobalParameter&) = delete;
	GlobalParameter& operator=(const GlobalParameter&) = delete;
};
