#pragma once
#include "3d/Object3d/Object3d.h"
#include "3d/WorldTransform.h"
#include "Easing/EasingSequence.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include <array>
#include <cstdint>
#include <json.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>

enum class EasingAdaptTransform {
    Scale,
    Rotate,
    Translate,
};

/// <summary>
/// Blenderからエクスポートしたオブジェクト配置クラス
/// </summary>
namespace KetaEngine {

class ObjectFromBlender {
public:
    struct LevelData {
        struct ObjectData {
            std::string fileName;
            std::unique_ptr<Object3d> object3d;
            std::list<ObjectData> children;
            std::vector<std::unique_ptr<EasingSequence>> scalingEasing;
            std::vector<std::unique_ptr<EasingSequence>> rotationEasing;
            std::vector<std::unique_ptr<EasingSequence>> translationEasing;
            std::unique_ptr<ParticlePlayer> particlePlayer;
            std::string particleName;
            std::vector<Vector3> preScale;
            std::vector<Vector3> preRotation;
            std::vector<Vector3> preTranslation;
            std::vector<std::array<bool, 3>> isAdaptEasing;
            std::vector<float> easingStartTimes;
            size_t groupCount = 0;
        };
        std::vector<ObjectData> objects;
    };

public:
    ObjectFromBlender()  = default;
    ~ObjectFromBlender() = default;

    /// <summary>
    /// JSONファイルのロード
    /// </summary>
    /// <param name="name">ファイル名</param>
    void LoadJsonFile(const std::string& name);

    /// <summary>
    /// 指定グループのイージング更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    /// <param name="groupNum">グループ番号</param>
    void EasingUpdateSelectGroup(float deltaTime, int32_t groupNum);

    /// <summary>
    /// 指定グループのイージングリセット
    /// </summary>
    /// <param name="groupNum">グループ番号</param>
    void EasingResetSelectGroup(int32_t groupNum);

    /// <summary>
    /// JSONデータをオブジェクトに変換
    /// </summary>
    /// <param name="object">JSONオブジェクト</param>
    void ConvertJSONToObjects(const nlohmann::json& object);

    /// <summary>
    /// 文字列をプリミティブタイプに変換
    /// </summary>
    /// <param name="typeStr">タイプ文字列</param>
    /// <returns>プリミティブタイプ</returns>
    PrimitiveType StringToPrimitiveType(const std::string& typeStr);

    void EmitterAllUpdate(); //< 全エミッター更新
    void EmitAll(const std::string& particleName = ""); //< 全エミッター放出
  
    void EasingAllReset(); //< 全イージングリセット

private:
    /// <summary>
    /// イージングの適用
    /// </summary>
    /// <param name="objectData">オブジェクトデータ</param>
    /// <param name="groupNum">グループ番号</param>
    void AdaptEasing(LevelData::ObjectData& objectData, int32_t groupNum);

    /// <summary>
    /// イージンググループのロード
    /// </summary>
    /// <param name="easingGroups">イージンググループJSON</param>
    /// <param name="objectData">オブジェクトデータ</param>
    void LoadEasingGroups(const nlohmann::json& easingGroups, LevelData::ObjectData& objectData);

    /// <summary>
    /// イージングが適用されているか
    /// </summary>
    /// <param name="objectData">オブジェクトデータ</param>
    /// <param name="groupNum">グループ番号</param>
    /// <param name="type">変換タイプ</param>
    /// <returns>適用されているか</returns>
    bool IsAdaptEasing(const LevelData::ObjectData& objectData, int32_t groupNum, const EasingAdaptTransform& type) const;

private:
    const std::string directoryPath_ = "Resources/BlenderObjectPos/";
    std::string fileName_;
    std::unique_ptr<LevelData> levelData_;
    float currentTime_ = 0.0f;
    nlohmann::json jsonData_;

public:
    bool GetIsEasingFinish(int32_t groupNum) const;
    bool GetIsEasingPlaying(int32_t groupNum) const;

    void SetLoopEndCallback(int32_t groupNum, const EasingAdaptTransform& transformType, const std::function<void()>& callback);
};

}; // KetaEngine
