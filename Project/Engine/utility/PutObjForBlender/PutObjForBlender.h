#pragma once
#include <string>
#include <vector>
#include"3d/object3d.h"
#include"3d/WorldTransform.h"
#include <list>
#include <json.hpp>

class PutObjForBlender {
public:
    // レベルデータ
    struct LevelData {
        //オブジェクト1個分のデータ
        struct ObjectData {
            std::string fileName;
           WorldTransform worldTransform; 
           std::unique_ptr<Object3d> object3d;
           std::list<ObjectData> children;
        };
        //オブジェクトのコンテナ
        std::vector<ObjectData>objects;
    };

public:
    PutObjForBlender()  = default;
    ~PutObjForBlender() = default;

    // functions
    void LoadJsonFile(const std::string& name);
    void PutObject();
    void DrawAll(const ViewProjection& viewProjection);

    void ConvertJSONToObjects(const nlohmann::json& object);

private:
    void DrawObject(LevelData::ObjectData& objectData, const ViewProjection& viewProjection);

private:
    const std::string directoryPath_ = "Resources/BlenderObjectPos/";
    std::string fileName_;
    std::unique_ptr<LevelData> levelData_;
  
    // jsonデータ
    nlohmann::json jsonData_;

public:
};