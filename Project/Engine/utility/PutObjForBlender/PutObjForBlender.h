#pragma once
#include <string>
#include <vector>
#include"3d/object3d.h"
#include"3d/WorldTransform.h"
#include "Vector3.h"

class PutObjForBlender {
public:
    // レベルデータ
    struct LevelData {
        //オブジェクト1個分のデータ
        struct ObjectData {
            std::string fileName;
           WorldTransform worldTransform; 
           Object3d* object3d;
        };
        //オブジェクトのコンテナ
        std::vector<ObjectData>objects;
    };

public:
    PutObjForBlender()  = default;
    ~PutObjForBlender() = default;

    // json file load
    void LoadJsonFile(const std::string& name);

private:
    const std::string directoryPath_ = "Resources/BlenderObjectPos/";
    std::string fileName_;
    LevelData* levelData_;

public:
};