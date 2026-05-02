#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>
#include <array>
#include <string>

/// <summary>
/// 取り巻きがどのボスグループに属するかを示すアイコン
/// </summary>
class GroupIcon {
public:
    GroupIcon()  = default;
    ~GroupIcon() = default;

    static constexpr int32_t kGroupCount = 3;

    void Init();
    void Update(const Vector2& screenPos, bool isDraw);

    /// <summary>
    /// グループインデックスをセット
    /// </summary>
    void SetGroupIndex(int32_t index);

private:
    const std::array<std::string, kGroupCount> kTexturePaths = {
        "Enemy/GroupIconTriangle.dds",
        "Enemy/GroupIconCircle.dds",
        "Enemy/GroupIconCube.dds",
    };

    std::unique_ptr<KetaEngine::Sprite> sprite_;
    int32_t groupIndex_ = -1;
};
