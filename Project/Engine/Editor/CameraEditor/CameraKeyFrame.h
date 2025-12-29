#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include"utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector3.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

/// <summary>
/// カメラキーフレームクラス
/// </summary>
namespace KetaEngine {

class CameraKeyFrame {
public:
    struct KeyFrameParam {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        float fov        = 45.0f;
    };

public:
    CameraKeyFrame()  = default;
    ~CameraKeyFrame() = default;

    // 初期化、更新
    void Init(const std::string& groupName, int32_t keyNumber);
    void Update(float speedRate);

    // パラメータ調整、登録、適応
    void AdjustParam();           
    void RegisterParams();        
    void AdaptEaseParam();   

    void AdaptValueSetting(); //< 値設定適用
    void Reset();             //< リセット
    bool IsFinished() const;  //< 終了判定

    void LoadData(); //< データ読み込み
    void SaveData(); //< データ保存
private:
    void GetParams();

private:

    // GlobalParam
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "CameraAnimation/KeyFrames/";

    int32_t currenTSequenceElementIndex  = -1;

    float timePoint_ = 0.0f;
    KeyFrameParam keyFrameParam_;
    KeyFrameParam currenTSequenceElementParam_;

    int32_t positionEaseType_ = 0;
    int32_t rotationEaseType_ = 0;
    int32_t fovEaseType_      = 0;

    TimeModeSelector timeModeSelector_;
 
    Easing<Vector3> positionEase_;
    Easing<Vector3> rotationEase_;
    Easing<float> fovEase_;


public:
    float GetTimePoint() const { return timePoint_; }
    const Vector3& GetPosition() const { return currenTSequenceElementParam_.position; }
    const Vector3& GetRotation() const { return currenTSequenceElementParam_.rotation; }
    float GetFov() const { return currenTSequenceElementParam_.fov; }
    const Vector3& GetEditPosition() const { return keyFrameParam_.position; }
    const Vector3& GetEditRotation() const { return keyFrameParam_.rotation; }
    float GetEditFov() const { return keyFrameParam_.fov; }
   
    void SetTimePoint(float timePoint) { timePoint_ = timePoint; }
    void SetStartEasing(const Vector3& pos, const Vector3& rotate, float fov);
 
};

}; // KetaEngine
