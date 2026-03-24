#pragma once

#include "utility/ObjectFromBlender/ObjectFromBlender.h"
// std
#include <memory>
#include <string>

/// <summary>
/// リザルト画面のステージ
/// ObjectFromBlender でオブジェクトを配置する
/// </summary>
class ResultStage {
public:
    ResultStage()  = default;
    ~ResultStage() = default;

    void Init(const std::string& filename);
    void Update();

private:
    std::unique_ptr<KetaEngine::ObjectFromBlender> objectFromBlender_;
};
