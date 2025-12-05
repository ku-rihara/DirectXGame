#pragma once
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "Editor/RailEditor/Rail.h"
#include "RailControlPoint.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Line3D;

/// <summary>
/// レールデータ
/// </summary>
class RailData : public BaseSequenceEffectData<RailControlPoint> {
public:
    enum class PositionMode {
        WORLD,
        LOCAL,
    };

    enum class ReturnMode {
        NONE,
        REVERSE_RAIL,
        DIRECT_RETURN
    };

    struct RailDirectReturnParam {
        float maxTime       = 1.0f;
        int32_t easeTypeInt = 0;
        Easing<Vector3> ease;
        Vector3 easeAdaptPos;
    };

    struct RailMoveParam {
        ReturnMode returnMode = ReturnMode::NONE;
        float maxTime         = 1.0f;
        float startTime       = 0.0f;
        int32_t easeTypeInt   = 0;
        int32_t returnModeInt = 0;
        bool isLoop           = false;
        float elapsedTime     = 0.0f;
        Easing<float> timeEase;
        float adaptTime = 0.0f;
    };

public:
    RailData()           = default;
    ~RailData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& railName) override;
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;
    void LoadData() override;
    void SaveData() override;

    // BaseSequenceEffectDataからのオーバーライド
    void LoadSequenceElements() override;
    void SaveSequenceElements() override;

    // Rail固有の更新
    void UpdateWithDirection(float speedRate, const PositionMode& mode = PositionMode::WORLD, const Vector3& direction = {1.0f, 1.0f, 1.0f});

    void AdjustParam();

    void SetControlPointLines(Line3D* line3d, const Vector4& color = {1.0f, 1.0f, 0.0f, 1.0f});

protected:
    //*---------------------------- protected Methods ----------------------------*//

    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    void UpdateKeyFrameProgression() override;
    void AdvanceToNexTSequenceElement() override;
    std::unique_ptr<RailControlPoint> CreateKeyFrame(int32_t index) override;
    std::string GeTSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//

    void CheckAndHandleFinish();
    void StartReturn();
    void UpdateReturn(float speedRate);
    void OnReturnComplete();
  
private:
    //*---------------------------- private Variant ----------------------------*//

    std::string dateFolderPath_     = "RailEditor/Dates";
    const std::string keyFramePath_ = "RailEditor/ControlPoints/";
    std::unique_ptr<Rail> rail_;

    Vector3 currentPosition_ = Vector3::ZeroVector();
    Vector3 startPosition_   = Vector3::ZeroVector();
    Vector3 direction_       = {1.0f, 1.0f, 1.0f};

    WorldTransform* parentTransform_ = nullptr;

    RailDirectReturnParam directReturnParam_;
    RailMoveParam railMoveParam_;

    bool showControlPointLines_ = true;

    TimeModeSelector timeModeSelector_;

public:
    //*----------------------------- getter Methods -----------------------------*//

    const Vector3& GetCurrentPosition() const { return currentPosition_; }
    WorldTransform* GetParentTransform() const { return parentTransform_; }

    void SetParent(WorldTransform* parent) { parentTransform_ = parent; }
};