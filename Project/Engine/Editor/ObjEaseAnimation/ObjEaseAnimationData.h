#pragma once
#include "Easing/Easing.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationKeyFrame.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class WorldTransform;
class RailPlayer;

/// <summary>
/// オブジェクトイージングアニメーションデータ
/// </summary>
class ObjEaseAnimationData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        RETURNING
    };

    enum class TransformType {
        Scale,
        Rotation,
        Translation,
        Count
    };

    struct KeyFrameState {
        int selectedKeyFrameIndex      = -1;
        int activeKeyFrameIndex        = 0;
        bool isAllKeyFramesFinished    = false;
    };

public:
    ObjEaseAnimationData()  = default;
    ~ObjEaseAnimationData() = default;

    // 初期化、更新
    void Init(const std::string& animationName, const std::string& categoryName);
    void Update();

    // 再生制御
    void Play();
    void Stop();
    void Reset();

    // 編集、セーブ、ロード
    void AdjustParam();
    void LoadData();
    void SaveData();

    // 再生状態
    bool IsPlaying() const;
    bool IsFinished() const;

private:
    const char* GetSRTName(const TransformType& type) const;

    // パラメータ登録、取得、リセット
    void RegisterParams();
    void GetParams();
    void InitParams();

    // キーフレーム更新
    void UpdateKeyFrameProgression();
    void AdvanceToNextKeyFrame();

    // キーフレームセーブ、ロード
    void LoadKeyFrames();
    void SaveKeyFrames();
    void CreateOrLoadKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles);

    // キーフレーム追加、削除、クリア
    void AddKeyFrame();
    void RemoveKeyFrame(const int32_t& index);
    void ClearKeyFrames();

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string categoryName_;
    std::string folderPath_;
    std::string keyFrameFolderPath_;
    const std::string baseFolderPath_ = "ObjEaseAnimation/";

    // keyFrame
    std::vector<std::unique_ptr<ObjEaseAnimationKeyFrame>> keyFrameDates_;
    KeyFrameState keyFrameState_;

    // originParam
    std::array<Vector3, static_cast<size_t>(TransformType::Count)> originalValues_;

    // state
    PlayState playState_ = PlayState::STOPPED;
    bool showControls_   = true;

public:
    const std::string& GetGroupName() const { return groupName_; }
    const std::string& GetCategoryName() const { return categoryName_; }
    const Vector3& GetOriginalValue(const TransformType& type) const { return originalValues_[static_cast<size_t>(type)]; }
    Vector3 GetActiveKeyFrameValue(const TransformType& type) const;
    bool GetIsUseRailActiveKeyFrame() const;
    RailPlayer* GetCurrentRailPlayer() const;
};