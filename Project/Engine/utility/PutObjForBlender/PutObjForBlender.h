#pragma once
#include "3d/object3d.h"
#include "3d/WorldTransform.h"
#include "Easing/EasingSequence.h"
#include "utility/ParticleEditor/ParticleEmitter.h"
#include <array>
#include <cstdint>
#include <json.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>

class PutObjForBlender {
private:
    enum class EasingAdaptTransform {
        Scale,
        Rotate,
        Translate
    };

public:
    // レベルデータ
    struct LevelData {
        // オブジェクト1個分のデータ
        struct ObjectData {
            std::string fileName;
            std::unique_ptr<Object3d> object3d;
            std::list<ObjectData> children;
            std::vector<std::unique_ptr<EasingSequence>> scalingEasing;
            std::vector<std::unique_ptr<EasingSequence>> rotationEasing;
            std::vector<std::unique_ptr<EasingSequence>> translationEasing;
            std::vector<std::unique_ptr<ParticleEmitter>> emitters;
            std::vector<Vector3> preScale;
            std::vector<Vector3> preRotation;
            std::vector<Vector3> preTranslation;
            std::vector<std::array<bool, 3>> isAdaptEasing;
            std::vector<float> easingStartTimes;
        };
        // オブジェクトのコンテナ
        std::vector<ObjectData> objects;
    };

public:
    PutObjForBlender()  = default;
    ~PutObjForBlender() = default;
    // functions
    void LoadJsonFile(const std::string& name);
    void DrawAll(const ViewProjection& viewProjection);
    // emit
    void EmitterAllUpdate();
    void EmitAll();
    void StartRailEmitAll();
    void EmitterAllEdit();
    // easing
    void EasingAllReset();
    void EasingUpdateSelectGroup(const float& deltaTime, const int32_t& groupNum);
    void EasingResetSelectGroup(const int32_t& groupNum);
    void ConvertJSONToObjects(const nlohmann::json& object);
    PrimitiveType StringToPrimitiveType(const std::string& typeStr);

private:
    void DrawObject(LevelData::ObjectData& objectData, const ViewProjection& viewProjection);
    void AdaptEasing(LevelData::ObjectData& objectData, const int32_t& groupNum);
    void LoadEasingGroups(const nlohmann::json& easingGroups, LevelData::ObjectData& objectData);
    bool IsAdaptEasing(const LevelData::ObjectData& objectData, const int32_t& groupNum, const EasingAdaptTransform& type) const;

private:
    const std::string directoryPath_ = "Resources/BlenderObjectPos/";
    std::string fileName_;
    std::unique_ptr<LevelData> levelData_;
    float currentTime_ = 0.0f;
    // jsonデータ
    nlohmann::json jsonData_;

public:
    bool GetIsEasingFinish(const int32_t& groupNum)const;
    bool GetIsEasingPlaying(const int32_t& groupNum) const;
};