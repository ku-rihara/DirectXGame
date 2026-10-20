#pragma once

#include "MoraParser.h"
#include <vector>

namespace KetaEngine {

/// <summary>
/// 抑揚(声の高さの時間変化)
/// 東京式の平板型をベースに、句ごとのへの字型と文末の下降/疑問の上昇を付ける
/// </summary>
class ProsodyModel {
public:
    struct Param {
        double basePitch    = 125.0; //< 基準の高さ[Hz]
        double accentRise   = 0.15;  //< 句頭の低→高の上がり幅(割合)
        double declination  = 0.12;  //< 句内での自然な下降(割合)
        double phraseDrop   = 0.04;  //< 句が進むごとの下降(割合)
        double finalFall    = 0.15;  //< 文末の下降(割合)
        double questionRise = 0.45;  //< 疑問の上昇(割合)
    };

public:
    // モーラ列から各モーラ境界の目標の高さを計算
    void Setup(const std::vector<Mora>& moras);

    /// <summary>
    /// 高さを取得
    /// </summary>
    /// <param name="position">モーラ単位の位置(2.5なら3番目のモーラの中間)</param>
    double PitchAt(double position) const;

private:
    Param param_;
    std::vector<double> targets_; //< モーラiの開始時点の高さ(末尾に終端を1つ追加)

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
