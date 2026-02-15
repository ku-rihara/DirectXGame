#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector2.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <string>

/// <summary>
/// スプライトイージングアニメーションセクション
/// </summary>
namespace KetaEngine {

class SpriteEaseAnimationSection {
public:
    enum class PropertyType {
        Scale,
        Position,
        Rotation,
        Color,
        Alpha,
        Count
    };

    enum class PlayState {
        STOPPED,
        WAITING,
        PLAYING,
        RETURN_WAITING,
        RETURNING
    };

    enum class PropertyState {
        INACTIVE,
        WAITING,
        PLAYING,
        RETURN_WAITING,
        RETURNING,
        FINISHED
    };

    struct Vec2Param {
        bool isActive         = false;
        bool isReturnToOrigin = false;

        Vector2 endValue = Vector2::ZeroVector();

        float maxTime    = 1.0f;
        int32_t easeType = 0;

        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        Vector2 currentOffset  = Vector2::ZeroVector();
        Vector2 startValue     = Vector2::ZeroVector();
        Vector2 previousOffset = Vector2::ZeroVector();

        Vector2 preAnimationOffset = Vector2::ZeroVector();

        Easing<Vector2> ease;
        Easing<Vector2> returnEase;

        PropertyState state    = PropertyState::INACTIVE;
        float returnElapsedTime = 0.0f;
    };

    struct Vec3Param {
        bool isActive         = false;
        bool isReturnToOrigin = false;

        Vector3 endValue = Vector3::ZeroVector();

        float maxTime    = 1.0f;
        int32_t easeType = 0;

        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        Vector3 currentOffset  = Vector3::ZeroVector();
        Vector3 startValue     = Vector3::ZeroVector();
        Vector3 previousOffset = Vector3::ZeroVector();

        Vector3 preAnimationOffset = Vector3::ZeroVector();

        Easing<Vector3> ease;
        Easing<Vector3> returnEase;

        PropertyState state    = PropertyState::INACTIVE;
        float returnElapsedTime = 0.0f;
    };

    struct FloatParam {
        bool isActive         = false;
        bool isReturnToOrigin = false;

        float endValue = 1.0f;

        float maxTime    = 1.0f;
        int32_t easeType = 0;

        float returnMaxTime    = 1.0f;
        int32_t returnEaseType = 0;

        float currentOffset  = 1.0f;
        float startValue     = 1.0f;
        float previousOffset = 1.0f;

        float preAnimationOffset = 1.0f;

        Easing<float> ease;
        Easing<float> returnEase;

        PropertyState state    = PropertyState::INACTIVE;
        float returnElapsedTime = 0.0f;
    };

public:
    SpriteEaseAnimationSection()  = default;
    ~SpriteEaseAnimationSection() = default;

    void Init(const std::string& animationName, const std::string& categoryName, int32_t keyNumber);
    void Update(float speedRate = 1.0f);

    void Reset();
    bool IsFinished() const;

    bool IsPropertyFinished(PropertyType type) const;
    void SetStartValueForProperty_Vec2(PropertyType type, const Vector2& value);
    void SetStartValueForProperty_Vec3(PropertyType type, const Vector3& value);
    void SetStartValueForProperty_Float(PropertyType type, float value);
    void StartWaitingForProperty(PropertyType type);
    void StartPlayingForProperty(PropertyType type);

    bool IsPropertyActive(PropertyType type) const;

    void LoadData();
    void SaveData();
    void AdjustParam();

    void SetStartValues(const Vector2& scale, const Vector2& position, const Vector3& rotation,
                        const Vector3& color, float alpha);
    void SetPreAnimationOffsets(const Vector2& scale, const Vector2& position, const Vector3& rotation,
                               const Vector3& color, float alpha);
    void StartWaiting();

private:
    const char* GetPropertyName(PropertyType type) const;

    void RegisterParams();
    void GetParams();
    void AdaptEaseParam();
    void AdaptValueSetting();

    void StartPlay();

    void UpdateVec2Property(Vec2Param& param, float deltaTime);
    void UpdateVec2Play(Vec2Param& param, float deltaTime);
    void UpdateVec2Return(Vec2Param& param, float deltaTime);

    void UpdateVec3Property(Vec3Param& param, float deltaTime);
    void UpdateVec3Play(Vec3Param& param, float deltaTime);
    void UpdateVec3Return(Vec3Param& param, float deltaTime);

    void UpdateFloatProperty(FloatParam& param, float deltaTime);
    void UpdateFloatPlay(FloatParam& param, float deltaTime);
    void UpdateFloatReturn(FloatParam& param, float deltaTime);

    void UpdateOverallState();
    bool AreAllPropertiesFinished() const;

    void ImGuiVec2Param(const char* label, Vec2Param& param);
    void ImGuiVec3Param(const char* label, Vec3Param& param, bool isRotation);
    void ImGuiFloatParam(const char* label, FloatParam& param, bool isAlpha);
    void ImGuiColorParam(const char* label, Vec3Param& param);

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_;
    int32_t currenTSequenceElementIndex_ = -1;

    // Vec2 params: Scale, Position
    Vec2Param scaleParam_;
    Vec2Param positionParam_;

    // Vec3 params: Rotation, Color
    Vec3Param rotationParam_;
    Vec3Param colorParam_;

    // Float param: Alpha
    FloatParam alphaParam_;

    // time
    float startTime_       = 0.0f;
    float returnStartTime_ = 0.0f;

    float elapsedTime_ = 0.0f;

    PlayState playState_ = PlayState::STOPPED;

    TimeModeSelector timeModeSelector_;

public:
    float GetTimePoint() const { return startTime_; }

    const Vector2& GetCurrentScale() const { return scaleParam_.currentOffset; }
    const Vector2& GetCurrentPosition() const { return positionParam_.currentOffset; }
    const Vector3& GetCurrentRotation() const { return rotationParam_.currentOffset; }
    const Vector3& GetCurrentColor() const { return colorParam_.currentOffset; }
    float GetCurrentAlpha() const { return alphaParam_.currentOffset; }

    const Vector2& GetEditScale() const { return scaleParam_.endValue; }
    const Vector2& GetEditPosition() const { return positionParam_.endValue; }
    const Vector3& GetEditRotation() const { return rotationParam_.endValue; }
    const Vector3& GetEditColor() const { return colorParam_.endValue; }
    float GetEditAlpha() const { return alphaParam_.endValue; }

    PlayState GetPlayState() const { return playState_; }
};

}; // KetaEngine
