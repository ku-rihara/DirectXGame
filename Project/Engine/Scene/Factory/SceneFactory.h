#pragma once

#include "AbstractSceneFactory.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

/// <summary>
/// シーン工場
/// </summary>
class SceneFactory : public AbstractSceneFactory {
public:
    ///============================================================
    /// public method
    ///============================================================

    SceneFactory();

    /// <summary>
    /// シーン作成
    /// </summary>
    /// <param name="sceneName">シーン名</param>
    /// <returns>作成したシーンのポインタ</returns>
    std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;

private:
   // シーン名,シーンの生成関数のテーブル
    std::unordered_map<std::string, std::function<std::unique_ptr<BaseScene>()>> sceneCreators_;
};
