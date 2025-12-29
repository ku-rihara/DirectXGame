#pragma once
#include "PostEffect/PostEffectRenderer.h"
#include "PostEffectData.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// ポストエフェクトコントローラー
/// </summary>
namespace KetaEngine {

class PostEffectController {
public:
    PostEffectController()  = default;
    ~PostEffectController() = default;

    // 初期化、更新
    void Init();
    void Update(float deltaTime);

    /// <summary>
    /// ポストエフェクトの再生
    /// </summary>
    /// <param name="postEffectName">エフェクト名</param>
    void PlayPostEffect(const std::string& postEffectName);

    /// <summary>
    /// ポストエフェクトの即座設定
    /// </summary>
    /// <param name="mode">エフェクトモード</param>
    void SetPostEffectImmediate(PostEffectMode mode);

    /// <summary>
    /// ポストエフェクトの追加
    /// </summary>
    /// <param name="postEffectName">エフェクト名</param>
    void AddPostEffect(const std::string& postEffectName);

    /// <summary>
    /// ポストエフェクトの削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemovePostEffect(const int& index);

    /// <summary>
    /// 名前からポストエフェクトデータを取得
    /// </summary>
    /// <param name="name">エフェクト名</param>
    /// <returns>ポストエフェクトデータ</returns>
    PostEffectData* GetPostEffectByName(const std::string& name);

    PostEffectMode GetCurrentPostEffect() const; //< 現在のエフェクト取得
    bool IsPostEffectActive() const; //< エフェクトがアクティブか
    void EditorUpdate(); //< エディタ更新
    void StopPostEffect(); //< エフェクト停止
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ
    int GetPostEffectCount() const { return static_cast<int>(postEffects_.size()); }

private:
    std::vector<std::unique_ptr<PostEffectData>> postEffects_;
    PostEffectRenderer* renderer_ = nullptr;
    int selectedIndex_            = -1;

    char nameBuffer_[128] = "";

    bool isPostEffectActive_    = false;
    float currentTimer_         = 0.0f;
    float targetDuration_       = 0.0f;
    PostEffectMode defaultMode_ = PostEffectMode::NONE;
};

}; // KetaEngine
