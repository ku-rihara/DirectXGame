#include "PutObjForBlender.h"
#include "3d/Model.h"
#include <cassert>
#include <fstream>
#include <json.hpp>

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

    // JSON文字列から解凍したデータ
    nlohmann::json jsonData;

    // 解凍
    file >> jsonData;

    // 正しいレベルデータファイルかチェック
    assert(jsonData.is_object());
    assert(jsonData.contains("name"));
    assert(jsonData["name"].is_string());

    //"name"を文字列として取得
    std::string name = jsonData["name"].get<std::string>();
    // 正しいレベルデータファイルかチェック
    assert(name.compare("scene") == 0);

    // レベルデータ格納用インスタンスを生成
    levelData_ = std::make_unique<LevelData>();

    //"object"の全オブジェクトを走査
    for (nlohmann::json& object : jsonData["objects"]) {
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
            nlohmann::json& transform = object["transform"];
            objectData.worldTransform.Init();
            // 平行移動
            objectData.worldTransform.translation_.x = (float)transform["translation"][0];
            objectData.worldTransform.translation_.y = (float)transform["translation"][2];
            objectData.worldTransform.translation_.z = (float)transform["translation"][1];

            // 回転角
            objectData.worldTransform.rotation_.x = -(float)transform["rotation"][0];
            objectData.worldTransform.rotation_.y = -(float)transform["rotation"][2];
            objectData.worldTransform.rotation_.z = -(float)transform["rotation"][1];

            // スケーリング
            objectData.worldTransform.scale_.x = (float)transform["scaling"][0];
            objectData.worldTransform.scale_.y = (float)transform["scaling"][2];
            objectData.worldTransform.scale_.z = (float)transform["scaling"][1];

        }

        // // オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
        //if (object.contains("children")) {
        //    for (nlohmann::json& child : object["children"]) {
        //       
        //       
        //    }
        //}
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