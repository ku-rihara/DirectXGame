#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <string>

/// <summary>
/// レール制御点クラス
/// </summary>
class RailControlPoint {
public:
    RailControlPoint()  = default;
    ~RailControlPoint() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="railName">レール名</param>
    /// <param name="keyNumber">キー番号</param>
    void Init(const std::string& railName, const int32_t& keyNumber);

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド
    void Reset(); //< リセット

    void LoadData(); //< データ読み込み
    void SaveData(); //< データ保存

private:
    void GetParams();

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "RailEditor/ControlPoints";
    int32_t currenTSequenceElementIndex  = -1;

    Vector3 position_ = {0.0f, 0.0f, 0.0f};

public:
    const Vector3& GetPosition() const { return position_; }
    void SetPosition(const Vector3& position) { position_ = position; }
};