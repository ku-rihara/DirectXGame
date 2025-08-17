#include "CameraAnimationSerializer.h"
#include <algorithm>
#include <fstream>
#include <iostream>

bool CameraAnimationSerializer::SaveAnimationsToJson(const std::vector<CameraAnimation>& animations, const std::string& filePath) {
    try {
        nlohmann::json json;
        json["version"]    = "1.0";
        json["animations"] = nlohmann::json::array();

        for (const auto& animation : animations) {
            json["animations"].push_back(AnimationToJson(animation));
        }

        std::ofstream file(filePath);
        if (!file.is_open()) {
            SetErrorMessage("Failed to open file for writing: " + filePath);
            return false;
        }

        file << json.dump(4);
        file.close();

        std::cout << "Saved animations to: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        SetErrorMessage("Error saving animations: " + std::string(e.what()));
        return false;
    }
}

bool CameraAnimationSerializer::LoadAnimationsFromJson(const std::string& filePath, std::vector<CameraAnimation>& animations) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            SetErrorMessage("Failed to open file for reading: " + filePath);
            return false;
        }

        nlohmann::json json;
        file >> json;
        file.close();

        if (!json.contains("animations")) {
            SetErrorMessage("Invalid animation file format: missing 'animations' key");
            return false;
        }

        animations.clear();

        for (const auto& animJson : json["animations"]) {
            animations.push_back(JsonToAnimation(animJson));
        }

        std::cout << "Loaded " << animations.size() << " animations from: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        SetErrorMessage("Error loading animations: " + std::string(e.what()));
        return false;
    }
}

bool CameraAnimationSerializer::ExportAnimationToJson(const CameraAnimation& animation, const std::string& filePath) {
    try {
        nlohmann::json json = AnimationToJson(animation);

        std::ofstream file(filePath);
        if (!file.is_open()) {
            SetErrorMessage("Failed to open file for writing: " + filePath);
            return false;
        }

        file << json.dump(4);
        file.close();

        std::cout << "Exported animation '" << animation.name << "' to: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        SetErrorMessage("Error exporting animation: " + std::string(e.what()));
        return false;
    }
}

bool CameraAnimationSerializer::ImportAnimationFromJson(const std::string& filePath, CameraAnimation& animation) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            SetErrorMessage("Failed to open file for reading: " + filePath);
            return false;
        }

        nlohmann::json json;
        file >> json;
        file.close();

        animation = JsonToAnimation(json);

        std::cout << "Imported animation '" << animation.name << "' from: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        SetErrorMessage("Error importing animation: " + std::string(e.what()));
        return false;
    }
}

nlohmann::json CameraAnimationSerializer::KeyFrameToJson(const CameraKeyFrame& keyFrame) {
    nlohmann::json json;
    json["timePoint"]          = keyFrame.timePoint;
    json["position"]           = {keyFrame.position.x, keyFrame.position.y, keyFrame.position.z};
    json["rotation"]           = {keyFrame.rotation.x, keyFrame.rotation.y, keyFrame.rotation.z};
    json["fov"]                = keyFrame.fov;
    json["positionEasingType"] = static_cast<int>(keyFrame.positionEasingType);
    json["rotationEasingType"] = static_cast<int>(keyFrame.rotationEasingType);
    json["fovEasingType"]      = static_cast<int>(keyFrame.fovEasingType);
    return json;
}

CameraKeyFrame CameraAnimationSerializer::JsonToKeyFrame(const nlohmann::json& json) {
    CameraKeyFrame keyFrame;
    keyFrame.timePoint = json.value("timePoint", 0.0f);

    if (json.contains("position") && json["position"].size() == 3) {
        keyFrame.position.x = json["position"][0];
        keyFrame.position.y = json["position"][1];
        keyFrame.position.z = json["position"][2];
    }

    if (json.contains("rotation") && json["rotation"].size() == 3) {
        keyFrame.rotation.x = json["rotation"][0];
        keyFrame.rotation.y = json["rotation"][1];
        keyFrame.rotation.z = json["rotation"][2];
    }

    keyFrame.fov                = json.value("fov", 45.0f * 3.141592654f / 180.0f);
    keyFrame.positionEasingType = static_cast<EasingType>(json.value("positionEasingType", static_cast<int>(EasingType::InOutSine)));
    keyFrame.rotationEasingType = static_cast<EasingType>(json.value("rotationEasingType", static_cast<int>(EasingType::InOutSine)));
    keyFrame.fovEasingType      = static_cast<EasingType>(json.value("fovEasingType", static_cast<int>(EasingType::InOutSine)));

    return keyFrame;
}

nlohmann::json CameraAnimationSerializer::AnimationToJson(const CameraAnimation& animation) {
    nlohmann::json json;
    json["name"]          = animation.name;
    json["totalDuration"] = animation.totalDuration;
    json["isLooping"]     = animation.isLooping;
    json["keyFrames"]     = nlohmann::json::array();

    for (const auto& keyFrame : animation.keyFrames) {
        json["keyFrames"].push_back(KeyFrameToJson(keyFrame));
    }

    return json;
}

CameraAnimation CameraAnimationSerializer::JsonToAnimation(const nlohmann::json& json) {
    CameraAnimation animation;
    animation.name          = json.value("name", "Unnamed");
    animation.totalDuration = json.value("totalDuration", 0.0f);
    animation.isLooping     = json.value("isLooping", false);

    if (json.contains("keyFrames")) {
        for (const auto& keyFrameJson : json["keyFrames"]) {
            animation.keyFrames.push_back(JsonToKeyFrame(keyFrameJson));
        }
    }

    // 総時間を再計算（安全のため）
    if (!animation.keyFrames.empty()) {
        std::sort(animation.keyFrames.begin(), animation.keyFrames.end(),
            [](const CameraKeyFrame& a, const CameraKeyFrame& b) {
                return a.timePoint < b.timePoint;
            });
        animation.totalDuration = animation.keyFrames.back().timePoint;
    }

    return animation;
}

void CameraAnimationSerializer::SetErrorMessage(const std::string& message) {
    lastErrorMessage_ = message;
    std::cerr << "CameraAnimationSerializer Error: " << message << std::endl;
}