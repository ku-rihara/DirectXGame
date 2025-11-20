#pragma once
#include "Easing/Easing.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationKeyFrame.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/FileSelector/FileSelector.h"
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
        int finalKeyFrameIndex;
        int selectedKeyFrameIndex      = -1;
        int lastCompletedKeyFrameIndex = -1;
        int activeKeyFrameIndex        = 0;
        bool isAllKeyFramesFinished    = false;
    };

    struct TransformParam {
        bool isActive       = false;
        bool returnToOrigin = false;

        Vector3 startValue = Vector3::ZeroVector();

        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        // 現在のオフセット値
        Vector3 currentOffset = Vector3::ZeroVector();
        Easing<Vector3> ease;
    };

    // Rail用ファイルセレクター
    struct RailFileSelector {
        FileSelector selector;
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
    void ResetParams();

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

    void StartReturn(); //< 戻り動作開始
    void CheckFinish(); //< 終了判定

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string categoryName_;
    std::string folderPath_;

    // keyFrame
    std::vector<std::unique_ptr<ObjEaseAnimationKeyFrame>> keyFrameDates_;
    const std::string& keyFrameFolderName_ = "KeyFrames";
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