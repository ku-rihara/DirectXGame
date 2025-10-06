#include "PutObjForBlender.h"
#include "3d/Model.h"
#include "mathFunction.h"
#include <cassert>
#include <fstream>
#include <unordered_set>

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

    // 全オブジェクトを走査
    for (const auto& object : jsonData_["objects"]) {
        ConvertJSONToObjects(object);
    }
}

void PutObjForBlender::ConvertJSONToObjects(const nlohmann::json& object) {

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

                    if (emitter.contains("particle_name") && emitter.contains("primitive_type")) {

                        std::string name             = emitter["particle_name"].get<std::string>();
                        std::string primitiveTypeStr = emitter["primitive_type"].get<std::string>();

                        //  PrimitiveType変換
                        PrimitiveType primitiveType = StringToPrimitiveType(primitiveTypeStr);

                        //  エミッターを追加
                        objectData.emitters.emplace_back(
                            ParticleEmitter::CreateParticlePrimitive(name, primitiveType, 1600));
                    }
                }
            }
            // Emitter位置をオブジェクトに
            for (std::unique_ptr<ParticleEmitter>& emitter : objectData.emitters) {
                emitter->SetTargetPosition(objectData.object3d->transform_.GetWorldPos());
            }
        }

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


void PutObjForBlender::LoadEasingGroups(const nlohmann::json& easingGroups, LevelData::ObjectData& objectData) {

    // 最大のgroup_idを見つける
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

    // 必要なサイズを一度に確保して初期化
    size_t requiredSize = static_cast<size_t>(maxGroupId) + 1;

    // 現在のサイズより大きい場合のみリサイズ
    if (objectData.scalingEasing.size() < requiredSize) {
        // リサイズと同時に新しい要素を初期化
        objectData.scalingEasing.resize(requiredSize);
        objectData.rotationEasing.resize(requiredSize);
        objectData.translationEasing.resize(requiredSize);
        objectData.isAdaptEasing.resize(requiredSize);
        objectData.easingStartTimes.resize(requiredSize, 0.0f);
        objectData.preScale.resize(requiredSize);
        objectData.preRotation.resize(requiredSize);
        objectData.preTranslation.resize(requiredSize);

        // リサイズで追加された要素のみを初期化
        for (size_t i = 0; i < requiredSize; ++i) {
            if (!objectData.scalingEasing[i]) {
                objectData.scalingEasing[i] = std::make_unique<EasingSequence>();
            }
            if (!objectData.rotationEasing[i]) {
                objectData.rotationEasing[i] = std::make_unique<EasingSequence>();
            }
            if (!objectData.translationEasing[i]) {
                objectData.translationEasing[i] = std::make_unique<EasingSequence>();
            }
        }
    }

    // 第二のパス: 各グループの設定を読み込み
    for (const auto& group : easingGroups) {

        if (!group.contains("group_id") || !group.contains("steps")) {
            continue;
        }

        int32_t groupId = group["group_id"].get<int32_t>();

        // 範囲チェック
        if (groupId < 0 || groupId >= static_cast<int32_t>(objectData.scalingEasing.size())) {
            continue;
        }

        objectData.isAdaptEasing[groupId].fill(false);

        // start_timeを取得
        float startTime = 0.0f;
        if (group.contains("start_time")) {
            startTime = group["start_time"].get<float>();
        }
        objectData.easingStartTimes[groupId] = startTime;

        // is_loopを取得
        bool isLoop = false;
        if (group.contains("is_loop")) {
            isLoop = group["is_loop"].get<bool>();
        }

        // EasingSequenceにループフラグを設定
        if (objectData.scalingEasing[groupId]) {
            objectData.scalingEasing[groupId]->SetLoop(isLoop);
        }
        if (objectData.rotationEasing[groupId]) {
            objectData.rotationEasing[groupId]->SetLoop(isLoop);
        }
        if (objectData.translationEasing[groupId]) {
            objectData.translationEasing[groupId]->SetLoop(isLoop);
        }

        // ステップを走査
        for (const auto& step : group["steps"]) {

            if (!step.contains("step_number") || !step.contains("files")) {
                continue;
            }

            for (const auto& file : step["files"]) {
                if (!file.contains("filename") || !file.contains("srt_type")) {
                    continue;
                }

                std::string filename = file["filename"].get<std::string>();
                std::string srtType  = file["srt_type"].get<std::string>();

                if (srtType == "Scale" && objectData.scalingEasing[groupId]) {

                    objectData.scalingEasing[groupId]->AddStep(filename, &objectData.preScale[groupId]);
                    objectData.isAdaptEasing[groupId][static_cast<int>(EasingAdaptTransform::Scale)] = true;

                } else if (srtType == "Rotation" && objectData.rotationEasing[groupId]) {

                    objectData.rotationEasing[groupId]->AddStep(filename, &objectData.preRotation[groupId]);
                    objectData.rotationEasing[groupId]->SetBaseValue(objectData.object3d->transform_.rotation_);
                    objectData.isAdaptEasing[groupId][static_cast<int>(EasingAdaptTransform::Rotate)] = true;

                } else if (srtType == "Transform" && objectData.translationEasing[groupId]) {

                    objectData.translationEasing[groupId]->AddStep(filename, &objectData.preTranslation[groupId]);
                    objectData.translationEasing[groupId]->SetBaseValue(objectData.object3d->transform_.GetWorldPos());
                    objectData.isAdaptEasing[groupId][static_cast<int>(EasingAdaptTransform::Translate)] = true;
                }
            }
        }
    }
}

void PutObjForBlender::EmitterAllUpdate() {

    for (auto& objectData : levelData_->objects) {
        for (std::unique_ptr<ParticleEmitter>& emitter : objectData.emitters) {
            emitter->Update();
        }
    }
}

void PutObjForBlender::EmitAll() {
    for (auto& objectData : levelData_->objects) {
        for (std::unique_ptr<ParticleEmitter>& emitter : objectData.emitters) {
            emitter->Emit();
        }
    }
}

void PutObjForBlender::StartRailEmitAll() {
    for (auto& objectData : levelData_->objects) {
        for (std::unique_ptr<ParticleEmitter>& emitter : objectData.emitters) {
            emitter->StartRailEmit();
        }
    }
}

void PutObjForBlender::EasingAllReset() {
    for (auto& objectData : levelData_->objects) {
        for (auto& easingSequence : objectData.scalingEasing) {
            if (easingSequence) {
                easingSequence->Reset();
            }
        }
        for (auto& easingSequence : objectData.rotationEasing) {
            if (easingSequence) {
                easingSequence->Reset();
            }
        }
        for (auto& easingSequence : objectData.translationEasing) {
            if (easingSequence) {
                easingSequence->Reset();
            }
        }
        for (int32_t i = 0; i < objectData.isAdaptEasing.size(); ++i) {

            AdaptEasing(objectData, i);
        }
    }
    currentTime_ = 0.0f;
}


void PutObjForBlender::AdaptEasing(LevelData::ObjectData& objectData, const int32_t& groupNum) {
    // グループ番号が有効範囲内かチェック
    if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.preScale.size())) {
        return;
    }

    // scale
    if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Scale)) {
        objectData.object3d->transform_.scale_ = objectData.preScale[groupNum];
    }

    // rotate
    if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Rotate)) {
        objectData.object3d->transform_.rotation_ = objectData.preRotation[groupNum];
    }

    // translate
    if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Translate)) {
        objectData.object3d->transform_.translation_ = objectData.preTranslation[groupNum];
    }

    // 変更を反映
    objectData.object3d->Update();
}

void PutObjForBlender::EmitterAllEdit() {
    std::unordered_set<std::string> processedParticleNames;

    for (auto& objectData : levelData_->objects) {
        for (std::unique_ptr<ParticleEmitter>& emitter : objectData.emitters) {
            const std::string& name = emitter->GetParticleName();

            if (processedParticleNames.count(name) > 0) {
                continue;
            }

            emitter->EditorUpdate();
            processedParticleNames.insert(name);
        }
    }
}

void PutObjForBlender::DrawObject(LevelData::ObjectData& objectData, const ViewProjection& viewProjection) {
    if (objectData.object3d) {
        objectData.object3d->transform_.UpdateMatrix();
        objectData.object3d->Draw(viewProjection);
    }
}

void PutObjForBlender::DrawAll(const ViewProjection& viewProjection) {
    if (!levelData_)
        return;

    for (auto& obj : levelData_->objects) {
        DrawObject(obj, viewProjection);
    }
}


void PutObjForBlender::EasingUpdateSelectGroup(const float& deltaTime, const int32_t& groupNum) {
    // 現在時間を自動的に加算
    currentTime_ += deltaTime;

    for (auto& objectData : levelData_->objects) {
        // 指定されたグループが存在するかチェック
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.easingStartTimes.size())) {
            continue;
        }

        // 開始時間を超えるまではreturn
        if (currentTime_ < objectData.easingStartTimes[groupNum]) {
            continue;
        }

        // 指定されたグループのイージングを更新
        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Scale) && objectData.scalingEasing[groupNum]) {
            objectData.scalingEasing[groupNum]->Update(deltaTime);
        }
        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Rotate) && objectData.rotationEasing[groupNum]) {
            objectData.rotationEasing[groupNum]->Update(deltaTime);
        }
        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Translate) && objectData.translationEasing[groupNum]) {
            objectData.translationEasing[groupNum]->Update(deltaTime);
        }

        // PreValueをWorldTransformに適用
        AdaptEasing(objectData, groupNum);
    }
}

void PutObjForBlender::EasingResetSelectGroup(const int32_t& groupNum) {
    for (auto& objectData : levelData_->objects) {
        // 指定されたグループが存在するかチェック
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.scalingEasing.size())) {
            continue;
        }

        // 指定されたグループのイージングをリセット
        if (objectData.scalingEasing[groupNum]) {
            objectData.scalingEasing[groupNum]->Reset();
        }
        if (objectData.rotationEasing[groupNum]) {
            objectData.rotationEasing[groupNum]->Reset();
        }
        if (objectData.translationEasing[groupNum]) {
            objectData.translationEasing[groupNum]->Reset();
        }

        // リセット後の初期値を適用
        AdaptEasing(objectData, groupNum);
    }
}

bool PutObjForBlender::GetIsEasingPlaying(const int32_t& groupNum) const {
    if (!levelData_) {
        return false;
    }

    for (const auto& objectData : levelData_->objects) {
        // 指定されたグループが存在するかチェック
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.scalingEasing.size())) {
            continue;
        }

        // いずれかのイージングが終了していない場合はfalseを返す
        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Scale)) {
            if (objectData.scalingEasing[groupNum] && objectData.scalingEasing[groupNum]->IsAllPlaying()) {
                return true;
            }
        }

        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Rotate)) {
            if (objectData.rotationEasing[groupNum] && objectData.rotationEasing[groupNum]->IsAllPlaying()) {
                return true;
            }
        }

        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Translate)) {
            if (objectData.translationEasing[groupNum] && objectData.translationEasing[groupNum]->IsAllPlaying()) {
                return true;
            }
        }
    }

    // 何かしら再生中
    return false;
 }

bool PutObjForBlender::GetIsEasingFinish(const int32_t& groupNum) const {
    if (!levelData_) {
        return true;
    }

    for (const auto& objectData : levelData_->objects) {
        // 指定されたグループが存在するかチェック
        if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.scalingEasing.size())) {
            continue;
        }

        // いずれかのイージングが終了していない場合はfalseを返す
        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Scale)) {
            if (objectData.scalingEasing[groupNum] && !objectData.scalingEasing[groupNum]->IsAllFinished()) {
                return false;
            }
        }

        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Rotate)) {
            if (objectData.rotationEasing[groupNum] && !objectData.rotationEasing[groupNum]->IsAllFinished()) {
                return false;
            }
        }

        if (IsAdaptEasing(objectData, groupNum, EasingAdaptTransform::Translate)) {
            if (objectData.translationEasing[groupNum] && !objectData.translationEasing[groupNum]->IsAllFinished()) {
                return false;
            }
        }
    }

    // すべてのオブジェクトの指定グループのイージングが完了している
    return true;
}


bool PutObjForBlender::IsAdaptEasing(const LevelData::ObjectData& objectData, const int32_t& groupNum, const EasingAdaptTransform& type)const {
    if (groupNum < 0 || groupNum >= static_cast<int32_t>(objectData.isAdaptEasing.size())) {
        return false;
    }
    return objectData.isAdaptEasing[groupNum][static_cast<int32_t>(type)];
}

PrimitiveType PutObjForBlender::StringToPrimitiveType(const std::string& typeStr) {
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