#pragma once
#include "CameraEditor.h"
#include <json.hpp>
#include <string>
#include <vector>

// 前方宣言
struct CameraKeyFrame;
struct CameraAnimation;

class CameraAnimationSerializer {
public:
    CameraAnimationSerializer()  = default;
    ~CameraAnimationSerializer() = default;

    /// 全アニメーションをJSONファイルに保存
    bool SaveAnimationsToJson(const std::vector<CameraAnimation>& animations, const std::string& filePath);

    /// JSONファイルから全アニメーションを読み込み
    bool LoadAnimationsFromJson(const std::string& filePath, std::vector<CameraAnimation>& animations);

    /// 現在のアニメーションをJSONファイルにエクスポート
    bool ExportAnimationToJson(const CameraAnimation& animation, const std::string& filePath);

    /// JSONファイルからアニメーションをインポート
    bool ImportAnimationFromJson(const std::string& filePath, CameraAnimation& animation);

    /// 最後のエラーメッセージを取得
    const std::string& GetLastErrorMessage() const { return lastErrorMessage_; }

private:
    /// JSON変換関数
    nlohmann::json KeyFrameToJson(const CameraKeyFrame& keyFrame);
    CameraKeyFrame JsonToKeyFrame(const nlohmann::json& json);
    nlohmann::json AnimationToJson(const CameraAnimation& animation);
    CameraAnimation JsonToAnimation(const nlohmann::json& json);

    /// エラーメッセージ設定
    void SetErrorMessage(const std::string& message);

private:
    std::string lastErrorMessage_;
};