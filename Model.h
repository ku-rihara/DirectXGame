#pragma once
#include<string>
#include"Vector4.h"
#include"Vector3.h"
#include"Vector2.h"
#include"ModelData.h"

class Model{
private:
	ModelData modelData_;//構築するModelData
	std::vector<Vector4>positions;//位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
public:
	//シングルトンインスタンスの取得
	static Model* GetInstance();

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);


};

