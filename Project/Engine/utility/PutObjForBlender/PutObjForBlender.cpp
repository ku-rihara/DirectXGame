#include "PutObjForBlender.h"
#include"mathFunction.h"
#include "3d/Model.h"
#include <cassert>
#include <fstream>

void PutObjForBlender::LoadJsonFile(const std::string& _fileName) {
    fileName_ = _fileName;

    // 　フルパスを取得
    const std::string fullPath = directoryPath_ + fileName_;

    // ファイルを開くためのストリーム
    std::ifstream file;

    // ファイル開く
    file.open(fullPath);

    // ファイルが開けなかったらエラー
    if (!file.is_open()) {
        assert(0);
    }

    // 解凍
    file >> jsonData_;

    // 正しいレベルデータファイルかチェック
    assert(jsonData_.is_object());
    assert(jsonData_.contains("name"));
    assert(jsonData_["name"].is_string());

    //"name"を文字列として取得
    std::string name = jsonData_["name"].get<std::string>();
    // 正しいレベルデータファイルかチェック
    assert(name.compare("scene") == 0);

    // レベルデータ格納用インスタンスを生成
    levelData_ = std::make_unique<LevelData>();

    //全オブジェクトを走査
    for (const auto& object : jsonData_["objects"]) {
        ConvertJSONToObjects(object);
    }
}

void PutObjForBlender::ConvertJSONToObjects(const nlohmann::json& object) {

    assert(object.contains("type"));

    // 種別を取得
    std::string type = object["type"].get<std::string>();

    // 種類ごとの処理
    if (type.compare("MESH") == 0) {

        // 要素追加
        levelData_->objects.emplace_back(LevelData::ObjectData{});
        // 追加した要素の参照を取得
        LevelData::ObjectData& objectData = levelData_->objects.back();

        if (object.contains("file_name")) {
            // ファイル名
            objectData.fileName = object["file_name"].get<std::string>();
        }

        // トランスフォームのパラメータ読み込み
        const nlohmann::json& transform = object["transform"];
        objectData.worldTransform.Init();
        // 平行移動
        objectData.worldTransform.translation_.x = (float)transform["translation"][0];
        objectData.worldTransform.translation_.y = (float)transform["translation"][2];
        objectData.worldTransform.translation_.z = (float)transform["translation"][1];

        // 回転角
        objectData.worldTransform.rotation_.x = -toRadian((float)transform["rotation"][0]);
        objectData.worldTransform.rotation_.y = -toRadian((float)transform["rotation"][2]);
        objectData.worldTransform.rotation_.z = -toRadian((float)transform["rotation"][1]);

        // スケーリング
        objectData.worldTransform.scale_.x = (float)transform["scaling"][0];
        objectData.worldTransform.scale_.y = (float)transform["scaling"][2];
        objectData.worldTransform.scale_.z = (float)transform["scaling"][1];
    }

    // 子要素があるなら再帰呼び出し
    if (object.contains("children") && object["children"].is_array()) {
        for (const auto& child : object["children"]) {
            ConvertJSONToObjects(child);
        }
    }
}
void PutObjForBlender::PutObject() {
    assert(levelData_); // LoadJsonFile 呼び出し前提

    for (auto& objectData : levelData_->objects) {
        objectData.object3d.reset(Object3d::CreateModel(objectData.fileName));
        assert(objectData.object3d);
    }
}

void PutObjForBlender::DrawObject(LevelData::ObjectData& objectData, const ViewProjection& viewProjection) {
    if (objectData.object3d) {
        objectData.worldTransform.UpdateMatrix();
        objectData.object3d->Draw(objectData.worldTransform, viewProjection);
    }
}

void PutObjForBlender::DrawAll(const ViewProjection& viewProjection) {
    if (!levelData_)
        return;

    for (auto& obj : levelData_->objects) {
        DrawObject(obj, viewProjection);
    }
}