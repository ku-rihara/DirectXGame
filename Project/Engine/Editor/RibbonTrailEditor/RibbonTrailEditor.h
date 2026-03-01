#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "RibbonTrailData.h"
#include "Vector3.h"
#include <memory>
#include <string>

namespace KetaEngine {

class RibbonTrail; // 前方宣言

/// <summary>
/// リボントレイルエディタクラス
///
/// [Fix] 旧実装はパラメータ表示UIのみで、プレビュー用 RibbonTrail オブジェクトが
///       存在しなかったためエディタ上でトレイルが一切表示されていなかった。
///       Update() 内で円運動のダミーパスを生成し、RibbonTrail にポイントを追加する
///       プレビューロジックを追加した。
/// </summary>
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

    // 前フレームで選択していたエフェクト名（切り替え検知用）
    std::string  prevSelectedName_;
};

} // KetaEngine
