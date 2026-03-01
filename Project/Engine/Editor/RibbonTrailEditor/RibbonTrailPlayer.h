#pragma once
#include "RibbonTrailData.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <memory>
#include <string>

namespace KetaEngine {

class RibbonTrail; // 前方宣言

/// <summary>
/// リボントレイル再生クラス
/// RibbonTrailData をロードして RibbonTrail オブジェクトを管理する
///
/// [Fix] 旧実装は Data の Getter のみ持ち、実際の RibbonTrail オブジェクトを
///       生成・更新・接続するコードが存在しなかった。
///       本クラスが RibbonTrail の生存期間を管理し、
///       Emit() でポイント追加、SyncDataToTrail() で設定反映を行う。
/// </summary>
class RibbonTrailPlayer : public BaseEffectPlayer {
public:
    RibbonTrailPlayer()           = default;
    ~RibbonTrailPlayer() override = default;

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    /// <summary>
    /// 指定プリセットをロードして再生状態にする
    /// [Fix] Data ロード後に RibbonTrail を生成し設定を反映するよう修正
    /// </summary>
    void Play(const std::string& presetName, const std::string& categoryName = "Common") override;

    /// <summary>
    /// [New] 毎フレーム呼び出す。位置を渡してEmitInterval毎にポイントを追加する。
    /// 呼び出し側（武器・手のスクリプト等）は Update() 後にこれを呼ぶ。
    /// </summary>
    void Emit(const Vector3& position, float deltaTime);

    /// <summary>
    /// [New] トレイル本体を取得（描画は RibbonTrailManager が自動で行う）
    /// </summary>
    RibbonTrail* GetTrail() const { return trail_; }

    /// <summary>
    /// トレイルをクリアして非表示にする
    /// </summary>
    void StopAndClear();

protected:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    RibbonTrailData* GetData() const;

    /// <summary>
    /// [New] Data のパラメータを Trail に反映する
    /// Play() 時と、エディタでパラメータ変更時に呼ぶ
    /// </summary>
    void SyncDataToTrail();

private:
    // [New] 実際の描画オブジェクト（生存期間をここで管理）
    RibbonTrail* trail_      = nullptr;
    float        emitTimer_  = 0.0f;

public:
    ///========================================================
    /// Getter（RibbonTrailData の各パラメータを転送）
    ///========================================================
    Vector4            GetStartColor()   const;
    Vector4            GetEndColor()     const;
    float              GetStartWidth()   const;
    float              GetEndWidth()     const;
    float              GetLifetime()     const;
    int32_t            GetMaxPoints()    const;
    float              GetEmitInterval() const;
    const std::string& GetTexturePath()  const;
};

} // KetaEngine
