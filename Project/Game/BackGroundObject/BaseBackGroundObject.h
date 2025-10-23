#pragma once
#include "utility/PutObjForBlender/PutObjForBlender.h"
#include <memory>
#include <string>

/// <summary>
/// 背景オブジェクトの基底クラス
/// </summary>
class BaseBackGroundObject {
public:
    BaseBackGroundObject()          = default;
    virtual ~BaseBackGroundObject() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="filename">ファイル名</param>
    virtual void Init(const std::string& filename);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    virtual void Update(const float& playSpeed) = 0;

protected:
    std::unique_ptr<PutObjForBlender> putObjForBlender_;
};