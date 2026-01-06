#pragma once
#include "../Parameter/ParticleCommonParameters.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "MinMax.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <functional>
#include <string>
#include <vector>

namespace KetaEngine {

/// <summary>
/// UV、色、テクスチャなど視覚関連のパラメータ
/// </summary>
class VisualParameters {
public:
    using UVParam = ParticleCommon::UVParam;

    VisualParameters()  = default;
    ~VisualParameters() = default;

    void RegisterParams(GlobalParameter* globalParam, const std::string& groupName);
    void AdaptParameters(GlobalParameter* globalParam, const std::string& groupName);
    void AdjustParam();
    void ImGuiTextureSelection();
    void ApplyTexture(const std::string& textureName);
    void InitAdaptTexture();

    // Getter
    const Vector4& GetBaseColor() const { return baseColor_; }
    const V4MinMax& GetColorDist() const { return colorDist_; }
    const UVParam& GetUVParam() const { return uvParam_; }
    const std::string& GetSelectedTexturePath() const { return selectedTexturePath_; }

    // Setter
    void SetBaseColor(const Vector4& color) { baseColor_ = color; }
    void SetTextureChangedCallback(std::function<void()> callback) { onTextureChanged_ = callback; }
    void SetTexturePath(const std::string& path) { selectedTexturePath_ = path; }

private:
    Vector4 baseColor_{1.0f, 1.0f, 1.0f, 1.0f};
    V4MinMax colorDist_;
    UVParam uvParam_;
    std::string selectedTexturePath_;
    const std::string textureFilePath_ = "Resources/texture/Particle";
    std::function<void()> onTextureChanged_;
};

} // namespace KetaEngine