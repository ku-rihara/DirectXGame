#pragma once
#include <string>
#include <vector>
#include"3d/object3d.h"
#include"3d/WorldTransform.h"
#include"EasingSequence.h"
#include"utility/ParticleEditor/ParticleEmitter.h"
#include <list>
#include <json.hpp>
#include <cstdint>

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
           std::vector<EasingSequence<Vector3>> scalingEasing;
           std::vector<EasingSequence<Vector3>> rotationEasing;
           std::vector<EasingSequence<Vector3>> translationEasing;
           std::vector<std::unique_ptr<ParticleEmitter>> emitters;
           Vector3 preScale;
           Vector3 preRotation;
           Vector3 preTranslation;
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

    // emit
    void EmitterAllUpdate();
    void EmitAll();
    void StartRailEmitAll();
    void EmitterAllEdit();

    // easing
    void EasingAllReset();
    void EasingUpdateSelectGroup(const float&deltaTime,const int32_t& groupNum);
  

    void ConvertJSONToObjects(const nlohmann::json& object);
    PrimitiveType StringToPrimitiveType(const std::string& typeStr);

private:
    void DrawObject(LevelData::ObjectData& objectData, const ViewProjection& viewProjection);
    void AdaptEasing(LevelData::ObjectData& objectData);
    void LoadEasingGroups(const nlohmann::json& easingGroups, LevelData::ObjectData& objectData);

private:
    const std::string directoryPath_ = "Resources/BlenderObjectPos/";
    std::string fileName_;
    std::unique_ptr<LevelData> levelData_;
  
    // jsonデータ
    nlohmann::json jsonData_;

public:
};