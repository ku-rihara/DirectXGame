#pragma once

#include "MoraParser.h"
#include "PhonemeTable.h"
#include "SpeechParams.h"
#include <vector>

namespace KetaEngine {

class ProsodyModel;

/// <summary>
/// モーラ列を発声パラメータの時系列(SpeechSegment列)に変換する
/// 子音ごとの閉鎖・破裂・気息・摩擦・鼻音、フォルマント遷移、母音の無声化を扱う
/// </summary>
class SpeechPlanner {
public:
    struct Param {
        double moraDuration   = 0.14; //< 1拍の長さ[秒]
        double pauseDuration  = 0.22; //< 区切りの長さ[秒]
        double speed          = 1.0;  //< 話速(大きいほど速い)
        double consonantGain  = 1.0;  //< 子音ノイズの強さ
        double breathiness    = 0.04; //< 母音に混ぜる息の量
        bool enableDevoicing  = true; //< 母音の無声化(「です」の「す」等)
    };

public:
    std::vector<SpeechSegment> Plan(const std::vector<Mora>& moras, const PhonemeTable& table, const ProsodyModel& prosody);

private:
    //*---------------------------- 各モーラ ----------------------------*//
    void PlanNormal(size_t index);
    void PlanNasal(size_t index);
    void PlanGeminate(size_t index);
    void PlanLong(size_t index);
    void PlanPause(size_t index);

    // 子音部分(母音へ遷移し終わるまで)。使った秒数を返す
    double PlanConsonant(const ConsonantSpec& spec, const VowelTarget& vowel, const SpeechParams& vowelParams, bool isPalatal);

    // 母音の終わり際(次の子音に向けた遷移・減衰)。使う秒数を返す
    double PlanVowelTail(size_t index, const VowelTarget& vowel, const SpeechParams& vowelParams, bool emit);

    //*---------------------------- パラメータ作成 ----------------------------*//
    SpeechParams MakeVowel(const VowelTarget& vowel, bool isDevoiced) const;
    SpeechParams MakeNasalMurmur(double zero, double f2) const;

    // フォルマントF1～F3をlocusからvowelへratioだけ寄せた値
    SpeechParams WithLocus(const SpeechParams& base, const PhonemeTable::Locus& locus, double ratio) const;

    // 子音のノイズ共振を設定(0指定は母音から計算)
    void ApplyNoiseShape(const ConsonantSpec& spec, const VowelTarget& vowel, SpeechParams& params) const;

    bool IsDevoiced(size_t index) const;
    bool IsVoicelessConsonant(const std::string& consonant) const;
    double Ms(double ms) const;

    //*---------------------------- 出力 ----------------------------*//
    // currentからtargetへ変化する区間を追加
    void Emit(double seconds, const SpeechParams& target);
    // startからtargetへ変化する区間を追加(currentを無視して切り替える)
    void EmitFrom(double seconds, const SpeechParams& start, const SpeechParams& target);

private:
    Param param_;

    // Plan中の作業状態
    const std::vector<Mora>* moras_    = nullptr;
    const PhonemeTable* table_         = nullptr;
    const ProsodyModel* prosody_       = nullptr;
    std::vector<SpeechSegment> segments_;
    SpeechParams current_;
    double position_         = 0.0; //< モーラ単位の現在位置
    double moraLength_       = 0.1; //< 現在のモーラの長さ[秒]
    const VowelTarget* lastVowel_ = nullptr;

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
