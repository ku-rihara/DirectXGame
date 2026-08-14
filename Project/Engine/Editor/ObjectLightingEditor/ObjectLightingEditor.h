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

private:
    /// <summary>
    /// モデル名に対応する設定を取得。未登録なら現在のマテリアル値でシードしてGlobalParameterに登録する
    /// </summary>
    Settings& EnsureRegistered(const std::string& modelName, Object3d* seedSource);

    /// <summary>
    /// モデル名からGlobalParameterのグループ名(≒保存ファイル名)を生成する
    /// </summary>
    static std::string MakeGroupName(const std::string& modelName);

private:
    GlobalParameter* globalParameter_ = nullptr;
    std::unordered_map<std::string, Settings> settings_; //< モデル名 -> 設定
    std::string selectedModelName_;
};

}; // KetaEngine
