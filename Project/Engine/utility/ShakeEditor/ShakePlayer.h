#pragma once
#include "3d/Object3d.h"
#include "ShakeData.h"
#include "Vector3.h"
#include <memory>
#include <string>
#include <vector>

class ShakePlayer {
public:
    ShakePlayer()  = default;
    ~ShakePlayer() = default;

    /// 初期化・更新
    void Init();
    void Update(float deltaTime);
    void EditorUpdate();

    /// ファイル管理
    void AllLoadFile();
    void AllSaveFile();
    void AddShake(const std::string& shakeName);
    void RemoveShake(int index);

    /// 再生制御
    void Play(const std::string& shakeName);
    void PlaySelectedShake();
    void StopSelectedShake();
    void ResetSelectedShake();
    void StopAllShakes();

    /// 状態取得
    bool IsSelectedShakePlaying() const;
    bool IsSelectedShakeFinished() const;
    bool IsAnyShakePlaying() const;
    ShakeData* GetSelectedShake();
    ShakeData* GetShakeByName(const std::string& name);

    /// シェイク値取得
    Vector3 GetTotalShakeOffset() const;
    Vector3 GetSelectedShakeOffset() const;

private:
    /// 内部処理
    void UpdateShakes(float deltaTime);
    void UpdateTotalShakeOffset();

private:
    // シェイクデータ
    std::vector<std::unique_ptr<ShakeData>> shakes_;
    int selectedIndex_ = -1;

    // プレビュー用オブジェクト
    std::unique_ptr<Object3d> preViewObj_ = nullptr;

    // 合成シェイク値
    Vector3 totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

    // UI用バッファ
    char nameBuffer_[128] = "";

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    int GetSelectedIndex() const { return selectedIndex_; }
    size_t GetShakeCount() const { return shakes_.size(); }

    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetSelectedIndex(int index);
};