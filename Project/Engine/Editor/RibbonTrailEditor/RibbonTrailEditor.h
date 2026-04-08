#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "RibbonTrailData.h"
#include "Vector3.h"
#include <memory>
#include <string>

namespace KetaEngine {

class RibbonTrail;


class RibbonTrailEditor : public BaseEffectEditor<RibbonTrailData> {
public:
    RibbonTrailEditor()  = default;
    ~RibbonTrailEditor() = default;

    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;
    void PlaySelectedAnimation() override;

protected:
    std::unique_ptr<RibbonTrailData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderName() const override;

private:
    /// <summary>
    /// 選択中のエフェクトが切り替わったときにプレビュー用トレイルを再生成する
    /// </summary>
    void RebuildPreviewTrail();

    /// <summary>
    /// Data のパラメータをプレビュー用トレイルに毎フレーム反映する
    /// </summary>
    void SyncPreviewTrail(RibbonTrailData* data);

private:
    const std::string folderName_ = "RibbonTrail/";

    // [New] プレビュー用 RibbonTrail
    RibbonTrail* previewTrail_   = nullptr;
    float        previewAngle_   = 0.0f; // 円運動用角度（ラジアン）
    float        emitTimer_      = 0.0f; // EmitInterval タイマー

    // プレビュー設定
    Vector3      previewCenter_  = {0.0f, 0.0f, 0.0f}; // 円運動の中心点
    float        previewRadius_  = 1.5f;                // 円の半径

    // 前フレームで選択していたエフェクト名
    std::string  prevSelectedName_;
};

} // KetaEngine
