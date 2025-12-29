#pragma once
#include "BaseBackGroundObject.h"

/// <summary>
/// ゲーム用背景オブジェクトクラス
/// </summary>
class GameBackGroundObject : public BaseBackGroundObject {
public:
    GameBackGroundObject()          = default;
    virtual ~GameBackGroundObject() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void Init(const std::string& filename) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(float playSpeed) override;

private:
};