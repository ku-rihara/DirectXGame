#include "ObjectFromBlender.h"

using namespace KetaEngine;
#include "3d/Model.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
#include "Frame/Frame.h"
#include "mathFunction.h"
#include <cassert>
#include <fstream>
#include <unordered_set>

void ObjectFromBlender::LoadJsonFile(const std::string& _fileName) {
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

    // 全オブジェクトを走査
    for (const auto& object : jsonData_["objects"]) {
        ConvertJSONToObjects(object);
    }
}

void ObjectFromBlender::ConvertJSONToObjects(const nlohmann::json& object) {

    assert(object.contains("type"));

    if (object.contains("isDisable")) {
        bool disabled = object["isDisable"].get<bool>();
        if (disabled) {
            return;
        }
    }

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

        objectData.object3d.reset(Object3d::CreateModel(objectData.fileName));

        ///------------------------------Transform------------------------------
        const nlohmann::json& transform = object["transform"];
        objectData.object3d->transform_.Init();
        // 平行移動
        objectData.object3d->transform_.translation_.x = (float)transform["translation"][0];
        objectData.object3d->transform_.translation_.y = (float)transform["translation"][2];
        objectData.object3d->transform_.translation_.z = (float)transform["translation"][1];

        // 回転角
        objectData.object3d->transform_.rotation_.x = -ToRadian((float)transform["rotation"][0]);
        objectData.object3d->transform_.rotation_.y = -ToRadian((float)transform["rotation"][2]);
        objectData.object3d->transform_.rotation_.z = -ToRadian((float)transform["rotation"][1]);

        // スケーリング
        objectData.object3d->transform_.scale_.x = (float)transform["scaling"][0];
        objectData.object3d->transform_.scale_.y = (float)transform["scaling"][2];
        objectData.object3d->transform_.scale_.z = (float)transform["scaling"][1];

        objectData.object3d->transform_.UpdateMatrix();

        ///------------------------------Emitter------------------------------
        if (object.contains("emitters") && object["emitters"].is_array()) {

            if (!object["emitters"].empty()) {
                for (const auto& emitter : object["emitters"]) {

                    if (emitter.contains("particle_name")) {
                        objectData.particleName = emitter["particle_name"].get<std::string>();
                    }
                }
            }
        }

        objectData.particlePlayer = std::make_unique<ParticlePlayer>();
        objectData.particlePlayer->Init();
        objectData.particlePlayer->SetFollowingPos(&objectData.object3d->transform_.translation_);
        objectData.particlePlayer->SetParentTransform(&objectData.object3d->transform_);

        ///------------------------------Easing------------------------------
        if (object.contains("easing_groups") && object["easing_groups"].is_array()) {
            LoadEasingGroups(object["easing_groups"], objectData);
        }
    }

    // 子要素があるなら再帰呼び出し
    if (object.contains("children") && object["children"].is_array()) {
        for (const auto& child : object["children"]) {
            ConvertJSONToObjects(child);
        }
    }
}

void ObjectFromBlender::LoadEasingGroups(const nlohmann::json& easingGroups, LevelData::ObjectData& objectData) {

    // 最大のGroup_idを見つける
    int32_t maxGroupId = -1;
    for (const auto& group : easingGroups) {
        if (!group.contains("group_id")) {
            continue;
        }
        int32_t groupId = group["group_id"].get<int32_t>();
        if (groupId > maxGroupId) {
            maxGroupId = groupId;
        }
    }

    // グループが存在しない場合は終了
    if (maxGroupId < 0) {
        return;
    }

    // 必要なサイズを一度に確保
    size_t requiredSize   = static_cast<size_t>(maxGroupId) + 1;
    objectData.groupCount = requiredSize;

    objectData.animationNames.resize(requiredSize);
    objectData.easingStartTimes.resize(requiredSize, 0.0f);
    objectData.easingLoopFlags.resize(requiredSize, false);
    objectData.groupStarted.resize(requiredSize, false);
    objectData.loopEndCallbacks.resize(requiredSize);

    // 各グループの設定を読み込み
    for (const auto& group : easingGroups) {

        if (!group.contains("group_id") || !group.contains("animation_name")) {
            continue;
        }

        int32_t groupId = group["group_id"].get<int32_t>();

        if (groupId < 0 || groupId >= static_cast<int32_t>(requiredSize)) {
            continue;
        }

        objectData.animationNames[groupId] = group["animation_name"].get<std::string>();

        if (group.contains("start_time")) {
            objectData.easingStartTimes[groupId] = group["start_time"].get<float>();
        }

        if (group.contains("is_loop")) {
            objectData.easingLoopFlags[groupId] = group["is_loop"].get<bool>();
        }
    }
}

void ObjectFromBlender::EmitterAllUpdate() {

    for (auto& objectData : levelData_->objects) {
        objectData.particlePlayer->SetTargetPosition(objectData.object3d->transform_.GetWorldPos());
        objectData.particlePlayer->Update();
    }
}

void ObjectFromBlender::EmitAll(const std::string& particleName) {

    for (auto& objectData : levelData_->objects) {
        // 使用するパーティクル名を決定
        std::string useName = particleName.empty() ? objectData.particleName : particleName;

        if (useName.empty()) {
            continue;
        }

        objectData.particlePlayer->Play(objectData.particleName, "ObjectFromBlender");
    }
}

void ObjectFromBlender::EasingAllReset() {
    for (auto& objectData : levelData_->objects) {
        for (int32_t i = 0; i < static_cast<int32_t>(objectData.groupCount); ++i) {
            if (!objectData.groupStarted[i]) {
                continue;
            }
            objectData.object3d->transform_.StopObjEaseAnimation();
            objectData.groupStarted[i] = false;
        }
    }
    currentTime_ = 0.0f;
}

void ObjectFromBlender::EasingUpdateSelectGroup(float deltaTime, int32_t groupNum) {
    currentTime_ += deltaTime;

    for (auto& objectData : levelData_->objects) {
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.groupCount)) {
            continue;
        }

        if (currentTime_ < objectData.easingStartTimes[groupNum]) {
            continue;
        }

        if (!objectData.groupStarted[groupNum]) {
            // 初回: アニメーション再生開始
            auto& transform = objectData.object3d->transform_;
            transform.PlayObjEaseAnimation(objectData.animationNames[groupNum], "BlenderObj");

            auto* player = transform.GetObjEaseAnimationPlayer();
            player->SetLoop(objectData.easingLoopFlags[groupNum]);

            // 事前に登録されていたコールバックを設定
            if (objectData.loopEndCallbacks[groupNum]) {
                player->SetLoopEndCallback(objectData.loopEndCallbacks[groupNum]);
            }

            objectData.groupStarted[groupNum] = true;
        }

    }
}

void ObjectFromBlender::EasingResetSelectGroup(int32_t groupNum) {
    for (auto& objectData : levelData_->objects) {
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.groupCount)) {
            continue;
        }

        if (!objectData.groupStarted[groupNum]) {
            continue;
        }

        objectData.object3d->transform_.StopObjEaseAnimation();
        objectData.groupStarted[groupNum] = false;
    }
}

bool ObjectFromBlender::GetIsEasingPlaying(int32_t groupNum) const {
    if (!levelData_) {
        return false;
    }

    for (const auto& objectData : levelData_->objects) {
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.groupCount)) {
            continue;
        }
        if (!objectData.groupStarted[groupNum]) {
            continue;
        }
        auto* player = objectData.object3d->transform_.GetObjEaseAnimationPlayer();
        if (player && player->IsPlaying()) {
            return true;
        }
    }

    return false;
}

bool ObjectFromBlender::GetIsEasingFinish(int32_t groupNum) const {
    if (!levelData_) {
        return true;
    }

    for (const auto& objectData : levelData_->objects) {
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.groupCount)) {
            continue;
        }
        if (!objectData.groupStarted[groupNum]) {
            return false; 
        }
        auto* player = objectData.object3d->transform_.GetObjEaseAnimationPlayer();
        if (player && !player->IsFinished()) {
            return false;
        }
    }

    return true;
}

void ObjectFromBlender::SetLoopEndCallback(int32_t groupNum, const std::function<void()>& callback) {
    if (!levelData_) {
        return;
    }

    for (auto& objectData : levelData_->objects) {
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.groupCount)) {
            continue;
        }
        // コールバックを保存（Play前でも後でも対応）
        objectData.loopEndCallbacks[groupNum] = callback;
        // 既に再生開始済みならば即座に設定
        if (objectData.groupStarted[groupNum]) {
            auto* player = objectData.object3d->transform_.GetObjEaseAnimationPlayer();
            if (player) {
                player->SetLoopEndCallback(callback);
            }
        }
    }
}

PrimitiveType ObjectFromBlender::StringToPrimitiveType(const std::string& typeStr) {
    if (typeStr == "Plane") {
        return PrimitiveType::Plane;
    }
    if (typeStr == "Sphere") {
        return PrimitiveType::Sphere;
    }
    if (typeStr == "Cylinder") {
        return PrimitiveType::Cylinder;
    }
    if (typeStr == "Ring") {
        return PrimitiveType::Ring;
    }
    if (typeStr == "Box") {
        return PrimitiveType::Box;
    } else {
        return PrimitiveType::Plane;
    }
}
