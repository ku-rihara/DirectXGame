#pragma once
#include "Easing/Easing.h"
#include "RailControlPoint.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/Rail.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Line3D;

/// <summary>
/// レールデータ
/// </summary>
class RailData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED,
        RETURNING
    };

    enum class PositionMode {
        WORLD,
        LOCAL,
    };

    enum class ReturnMode {
        NONE, // 戻らない
        REVERSE_RAIL, // レールを逆走して戻る
        DIRECT_RETURN // 直接元の位置に戻る
    };

    struct ReturnParam {
        ReturnMode mode  = ReturnMode::NONE;
        int32_t modeInt  = 0;
        float maxTime    = 1.0f;
        int32_t easeType = 0;
        Easing<Vector3> ease;
        Vector3 easeAdaptPos;
    };

public:
    RailData()  = default;
    ~RailData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="railName">レール名</param>
    void Init(const std::string& railName);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speed">移動速度</param>
    /// <param name="mode">座標モード</param>
    /// <param name="direction">方向</param>
    void Update(const float& speed, const PositionMode& mode = PositionMode::WORLD, const Vector3& direction = {1.0f, 1.0f, 1.0f});

    void AdjustParam(); //< パラメータ調整

    void Play(); //< 再生
    void Stop(); //< 停止
    void Reset(); //< リセット

    void LoadData(); //< データロード
    void SaveData(); //< データセーブ

    void SaveKeyFrames(); //< 全キーフレーム保存
    void LoadKeyFrames(); //< 全キーフレーム読み込み

    bool IsPlaying() const; //< 再生中か
    bool IsFinished() const; //< 終了したか

    /// <summary>
    /// 制御点を繋ぐ線を描画
    /// </summary>
    /// <param name="line3d">Line3Dオブジェクト</param>
    /// <param name="color">線の色</param>
    void SetControlPointLines(Line3D* line3d, const Vector4& color = {1.0f, 1.0f, 0.0f, 1.0f});

    /// <summary>
    /// キーフレームの追加
    /// </summary>
    void AddKeyFrame();

    /// <summary>
    /// キーフレームの削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemoveKeyFrame(const int32_t& index);

    /// <summary>
    /// 全キーフレームクリア
    /// </summary>
    void ClearKeyFrames();

    /// <summary>
    /// キーフレーム初期化
    /// </summary>
    void InitKeyFrames();

private:
    void RegisterParams(); //< パラメータのバインド
    void LoadParams(); //< パラメータ取得
    void InitParams(); //< パラメータリセット
    void LoopOrStop(); //< ループまたは停止
    void StartReturn(); //< 戻り動作を開始
    void UpdateReturn(const float& speed); //< 戻り動作の更新
    void ImGuiKeyFrameList(); //< キーフレームリストのImGui
    void RebuildAndLoadAllKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles);
    void CheckAndHandleFinish();
    void OnReturnComplete();
    void EaseTimeSetup(const bool&isReverse);
    private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "RailEditor/Dates";

    std::unique_ptr<Rail> rail_;

    // キーフレームリスト
    std::vector<std::unique_ptr<RailControlPoint>> controlPoints_;
    int32_t selectedKeyFrameIndex_ = -1;

    float maxTime_    = 1.0f;
    float startTime_  = 0.0f;
    int32_t easeType_ = 0;
    bool isLoop_      = true;

    float elapsedTime_ = 0.0f;
    Easing<float> timeEase_;
    float easedTime_ = 0.0f;

    PlayState playState_ = PlayState::STOPPED;

    Vector3 currentPosition_ = Vector3::ZeroVector();
    Vector3 startPosition_   = Vector3::ZeroVector();
    Vector3 direction_       = {1.0f, 1.0f, 1.0f};

    WorldTransform* parentTransform_ = nullptr;

    bool showControls_     = true;
    bool showKeyFrameList_ = true;

    //  戻り動作用のパラメータ
    ReturnParam returnParam_;

    // 線描画の表示設定
    bool showControlPointLines_ = true;

public:
    const std::string& GetGroupName() const { return groupName_; }
    const Vector3& GetCurrentPosition() const { return currentPosition_; }
    const bool& GetIsLoop() const { return isLoop_; }
    Rail* GetRail() { return rail_.get(); }
    WorldTransform* GetParentTransform() const { return parentTransform_; }
    const std::vector<std::unique_ptr<RailControlPoint>>& GetKeyFrames() const { return controlPoints_; }
    const int32_t& GetKeyFrameCount() const { return static_cast<int32_t>(controlPoints_.size()); }
    const bool GetShowControlPointLines()& { return showControlPointLines_; }

    void SetIsLoop(const bool& loop) { isLoop_ = loop; }
    void SetParent(WorldTransform* parent) { parentTransform_ = parent; }
    void SetDirection(const Vector3& direction) { direction_ = direction; }
    void SetSelectedKeyFrameIndex(const int32_t& index);
    void SetShowControlPointLines(const bool& show) { showControlPointLines_ = show; }
};