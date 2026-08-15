#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <string>
#include <unordered_map>

namespace KetaEngine {

class Object3d;

/// <summary>
/// Obj3Dのライティングモードやライティングパラメータを編集するクラス
/// </summary>
class ObjectLightingEditor {
private:
    ObjectLightingEditor()                                          = default;
    ~ObjectLightingEditor()                                         = default;
    ObjectLightingEditor(const ObjectLightingEditor&)                = delete;
    ObjectLightingEditor& operator=(const ObjectLightingEditor&)     = delete;

    /// <summary>
    /// モデル単位で編集するパラメータ
    /// </summary>
    struct Settings {
        int32_t enableLighting        = 0;
        float environmentCoefficient = 0.0f;
    };

public:
    static ObjectLightingEditor* GetInstance();

    /// <summary>
    /// 対象オブジェクトのモデル名に対応する設定を登録・同期し、マテリアルへ反映する
    /// </summary>
    /// <param name="obj">同期対象のオブジェクト</param>
    void SyncObject(Object3d* obj);

    void DebugImGui(); //< ImGuiデバッグ表示

    /// <summary>
    /// 前のシーンの設定を破棄し、以後の読み込み先を新しいシーンのフォルダに切り替える
    /// </summary>
    /// <param name="sceneName">これから始まるシーンの名前</param>
    void OnSceneChanged(const std::string& sceneName);

private:
    /// <summary>
    /// モデル名に対応する設定を取得。未登録なら現在のシーンのファイルから読み込む
    /// </summary>
    Settings& EnsureRegistered(const std::string& modelName, Object3d* seedSource);

    /// <summary>
    /// モデル名からGlobalParameterのグループ名を生成する
    /// </summary>
    static std::string MakeGroupName(const std::string& modelName);

    /// <summary>
    /// 現在のシーン名から保存先フォルダ名生成する
    /// </summary>
    std::string MakeSceneFolder() const;

private:
    GlobalParameter* globalParameter_ = nullptr;
    std::unordered_map<std::string, Settings> settings_; 
    std::string selectedModelName_;
    std::string currentSceneName_; 
};

}; // KetaEngine
