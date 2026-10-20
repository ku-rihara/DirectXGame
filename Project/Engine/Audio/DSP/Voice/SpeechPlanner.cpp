#include "SpeechPlanner.h"

using namespace KetaEngine;
#include "ProsodyModel.h"
#include <algorithm>

using Manner = ConsonantSpec::Manner;
using Place  = ConsonantSpec::Place;

std::vector<SpeechSegment> SpeechPlanner::Plan(const std::vector<Mora>& moras, const PhonemeTable& table, const ProsodyModel& prosody) {
    moras_   = &moras;
    table_   = &table;
    prosody_ = &prosody;
    segments_.clear();
    position_   = 0.0;
    moraLength_ = param_.moraDuration / param_.speed;

    // 最初の母音の形で無音から始める
    lastVowel_ = table.FindVowel('a');
    for (const Mora& m : moras) {
        if (const VowelTarget* v = table.FindVowel(m.vowel)) {
            lastVowel_ = v;
            break;
        }
    }
    if (!lastVowel_) {
        return {};
    }
    current_ = MakeVowel(*lastVowel_, false).Silenced();
    EmitFrom(Ms(30), current_, current_);

    for (size_t i = 0; i < moras.size(); ++i) {
        position_ = static_cast<double>(i);
        switch (moras[i].kind) {
        case Mora::Kind::Normal:
            PlanNormal(i);
            break;
        case Mora::Kind::Nasal:
            PlanNasal(i);
            break;
        case Mora::Kind::Geminate:
            PlanGeminate(i);
            break;
        case Mora::Kind::Long:
            PlanLong(i);
            break;
        case Mora::Kind::Pause:
            PlanPause(i);
            break;
        }
    }

    // 余韻
    Emit(Ms(60), current_.Silenced());
    return segments_;
}

///============================================================
/// 各モーラ
///============================================================
void SpeechPlanner::PlanNormal(size_t index) {
    const Mora& mora       = (*moras_)[index];
    const VowelTarget* v   = table_->FindVowel(mora.vowel);
    if (!v) {
        return;
    }
    const bool isDevoiced = IsDevoiced(index);
    moraLength_           = param_.moraDuration / param_.speed * (isDevoiced ? 0.75 : 1.0);

    SpeechParams vowel        = MakeVowel(*v, isDevoiced);
    const ConsonantSpec* spec = mora.consonant.empty() ? nullptr : table_->FindConsonant(mora.consonant);

    // 摩擦音の後の無声化母音は、摩擦が弱まりながら続く(「す」「し」)
    if (isDevoiced && spec && (spec->manner == Manner::Fricative || spec->manner == Manner::Affricate)) {
        ApplyNoiseShape(*spec, *v, vowel);
        vowel.frication = spec->noiseGain * param_.consonantGain * 0.45;
    }

    double used = 0.0;
    if (spec) {
        used = PlanConsonant(*spec, *v, vowel, mora.isPalatal);
    } else if (current_.voicing < 0.05 && current_.aspiration < 0.05) {
        // 無音からの母音: 声の立ち上がり
        SpeechParams onset = vowel.Silenced();
        EmitFrom(Ms(25), onset, vowel);
        used = Ms(25);
    } else {
        // 母音から母音へ
        Emit(Ms(45), vowel);
        used = Ms(45);
    }

    const double tail   = PlanVowelTail(index, *v, vowel, false);
    const double steady = (std::max)(moraLength_ - used - tail, Ms(15));
    Emit(steady, vowel);
    PlanVowelTail(index, *v, vowel, true);

    lastVowel_ = v;
}

void SpeechPlanner::PlanNasal(size_t index) {
    moraLength_ = param_.moraDuration / param_.speed;

    // 「ん」は後ろの子音の位置に同化する
    double zero = 1500.0;
    double f2   = 1300.0;
    if (index + 1 < moras_->size()) {
        const ConsonantSpec* next = table_->FindConsonant((*moras_)[index + 1].consonant);
        if (next) {
            switch (next->place) {
            case Place::Labial:
                zero = 1100.0;
                f2   = 900.0;
                break;
            case Place::Alveolar:
            case Place::Postalveolar:
                zero = 1700.0;
                f2   = 1700.0;
                break;
            case Place::Velar:
                zero = 2000.0;
                f2   = 1900.0;
                break;
            default:
                break;
            }
        }
    }

    const SpeechParams murmur = MakeNasalMurmur(zero, f2);
    Emit(Ms(30), murmur);
    Emit((std::max)(moraLength_ - Ms(30), Ms(20)), murmur);
}

void SpeechPlanner::PlanGeminate(size_t index) {
    moraLength_ = param_.moraDuration / param_.speed;

    // 後ろが摩擦音なら摩擦を伸ばす(「さっさ」)、それ以外は無音で溜める(「きって」)
    if (index + 1 < moras_->size()) {
        const Mora& next          = (*moras_)[index + 1];
        const ConsonantSpec* spec = table_->FindConsonant(next.consonant);
        const VowelTarget* v      = table_->FindVowel(next.vowel);
        if (spec && v && spec->manner == Manner::Fricative) {
            SpeechParams f = WithLocus(MakeVowel(*v, false), table_->GetLocus(spec->place, *v), 0.2).Silenced();
            ApplyNoiseShape(*spec, *v, f);
            f.frication = spec->noiseGain * param_.consonantGain;
            Emit(Ms(25), f);
            Emit((std::max)(moraLength_ - Ms(25), Ms(10)), f);
            return;
        }
    }
    const SpeechParams closed = current_.Silenced();
    Emit(Ms(10), closed);
    Emit((std::max)(moraLength_ - Ms(10), Ms(10)), closed);
}

void SpeechPlanner::PlanLong(size_t index) {
    moraLength_ = param_.moraDuration / param_.speed;
    if (!lastVowel_ || current_.voicing < 0.05) {
        Emit(moraLength_, current_);
        return;
    }
    const SpeechParams vowel = MakeVowel(*lastVowel_, false);
    const double tail        = PlanVowelTail(index, *lastVowel_, vowel, false);
    Emit((std::max)(moraLength_ - tail, Ms(20)), vowel);
    PlanVowelTail(index, *lastVowel_, vowel, true);
}

void SpeechPlanner::PlanPause(size_t index) {
    moraLength_ = param_.pauseDuration * (*moras_)[index].pauseScale / param_.speed;
    Emit(moraLength_, current_.Silenced());
}

///============================================================
/// 子音
///============================================================
double SpeechPlanner::PlanConsonant(const ConsonantSpec& spec, const VowelTarget& vowel, const SpeechParams& vowelParams, bool isPalatal) {
    const double gain                = param_.consonantGain;
    const PhonemeTable::Locus locus  = table_->GetLocus(spec.place, vowel);
    const SpeechParams closed        = WithLocus(vowelParams, locus, 0.0).Silenced();
    const SpeechParams release       = WithLocus(vowelParams, locus, 0.35).Silenced();
    const bool wasSilent             = current_.voicing < 0.05 && current_.aspiration < 0.05 && current_.frication < 0.05;
    double used                      = 0.0;

    // 母音への遷移(拗音は硬口蓋の半母音を経由)
    auto toVowel = [&](double transitionMs) {
        if (isPalatal) {
            SpeechParams glide = WithLocus(vowelParams, table_->GetLocus(Place::Palatal, vowel), 0.0);
            glide.voicing      = vowelParams.voicing * 0.7;
            glide.frication    = current_.frication * 0.3;
            Emit(Ms(25), glide);
            used += Ms(25);
        }
        // 声の立ち上がりは素早く、フォルマントはゆっくり
        SpeechParams onset = current_.Lerp(vowelParams, 0.3);
        onset.voicing      = vowelParams.voicing * 0.8;
        onset.aspiration   = (std::max)(current_.aspiration * 0.4, vowelParams.aspiration);
        onset.frication    = (std::max)(current_.frication * 0.2, vowelParams.frication);
        onset.burst        = 0.0;
        Emit(Ms(12), onset);
        Emit((std::max)(Ms(transitionMs) - Ms(12), Ms(5)), vowelParams);
        used += Ms(transitionMs);
    };

    switch (spec.manner) {
    case Manner::Stop: {
        // 閉鎖(有声音は低い声帯振動だけが漏れる=ボイスバー)
        SpeechParams hold = closed;
        if (spec.isVoiced) {
            hold.voicing    = 0.25;
            hold.formant[0] = 200.0;
        }
        Emit(Ms(8), hold);
        Emit((std::max)(Ms(spec.closureMs) - Ms(8), 0.0), hold);
        used += Ms(spec.closureMs);

        // 破裂
        SpeechParams burst = hold;
        ApplyNoiseShape(spec, vowel, burst);
        burst.burst            = spec.burstGain * gain;
        SpeechParams burstEnd  = burst;
        burstEnd.burst        *= 0.25;
        EmitFrom(Ms(spec.burstMs), burst, burstEnd);
        used += Ms(spec.burstMs);

        // 気息(無声音: 声帯が振動する前の息)
        if (spec.noiseMs > 0.0) {
            SpeechParams aspiration = release;
            ApplyNoiseShape(spec, vowel, aspiration);
            aspiration.aspiration = spec.noiseGain * gain;
            aspiration.frication  = spec.burstGain * gain * 0.15;

            SpeechParams aspirationEnd = WithLocus(vowelParams, locus, 0.6).Silenced();
            ApplyNoiseShape(spec, vowel, aspirationEnd);
            aspirationEnd.aspiration = aspiration.aspiration * 0.6;

            EmitFrom(Ms(spec.noiseMs), aspiration, aspirationEnd);
            used += Ms(spec.noiseMs);
        }
        toVowel(spec.transitionMs);
        break;
    }

    case Manner::Fricative: {
        SpeechParams fric = WithLocus(vowelParams, locus, 0.2).Silenced();
        ApplyNoiseShape(spec, vowel, fric);
        fric.frication = spec.noiseGain * gain;
        if (spec.isVoiced) {
            fric.voicing = 0.35;
        }
        if (spec.place == Place::Labial) {
            fric.aspiration = 0.3 * gain; // 「ふ」は息の成分が多い
        }
        const double rise = Ms((std::min)(25.0, spec.noiseMs * 0.3));
        Emit(rise, fric);
        Emit((std::max)(Ms(spec.noiseMs) - rise, 0.0), fric);
        used += Ms(spec.noiseMs);
        toVowel(spec.transitionMs);
        break;
    }

    case Manner::Affricate: {
        SpeechParams hold = closed;
        if (spec.isVoiced) {
            hold.voicing    = 0.22;
            hold.formant[0] = 200.0;
        }
        Emit(Ms(6), hold);
        Emit((std::max)(Ms(spec.closureMs) - Ms(6), 0.0), hold);
        used += Ms(spec.closureMs);

        // 破裂からそのまま摩擦へ
        SpeechParams burst = hold;
        ApplyNoiseShape(spec, vowel, burst);
        burst.burst     = spec.burstGain * gain;
        burst.frication = spec.noiseGain * gain * 0.8;

        SpeechParams fric = WithLocus(vowelParams, locus, 0.2).Silenced();
        ApplyNoiseShape(spec, vowel, fric);
        fric.frication = spec.noiseGain * gain;
        fric.voicing   = spec.isVoiced ? 0.4 : 0.0;

        EmitFrom(Ms(spec.burstMs), burst, fric);
        Emit(Ms(spec.noiseMs), fric);
        used += Ms(spec.burstMs + spec.noiseMs);
        toVowel(spec.transitionMs);
        break;
    }

    case Manner::Aspirate: {
        // 母音の口の形のまま息だけ流す
        SpeechParams h = vowelParams.Silenced();
        h.aspiration   = spec.noiseGain * gain;

        // 「ひ」「ひゃ」は硬口蓋の摩擦音(ç)
        if (isPalatal || &vowel == table_->FindVowel('i')) {
            h.noiseFrequency = {3800.0, 5500.0};
            h.noiseBandwidth = {1500.0, 2500.0};
            h.frication      = 0.6 * gain;
        }
        Emit(Ms(20), h);
        Emit((std::max)(Ms(spec.noiseMs) - Ms(20), 0.0), h);
        used += Ms(spec.noiseMs);
        toVowel(spec.transitionMs);
        break;
    }

    case Manner::Nasal: {
        // 鼻音のハミング → 口を開いて母音へ
        const SpeechParams murmur = MakeNasalMurmur(spec.nasalZero, locus[1]);
        if (wasSilent) {
            EmitFrom(Ms(15), murmur.Silenced(), murmur);
        } else {
            Emit(Ms(25), murmur);
        }
        Emit(Ms(spec.noiseMs), murmur);
        used += Ms(spec.noiseMs) + (wasSilent ? Ms(15) : Ms(25));

        SpeechParams opened = release;
        opened.voicing      = (std::max)(vowelParams.voicing, 0.3) * 0.85;
        Emit(Ms(10), opened);
        used += Ms(10);
        toVowel(spec.transitionMs);
        break;
    }

    case Manner::Flap: {
        // 舌先が一瞬触れて、F1と振幅が下がる
        SpeechParams tap = WithLocus(vowelParams, locus, 0.0);
        tap.formant[0]   = 300.0;
        tap.formant[2]   = 2500.0;
        tap.voicing      = 0.45;
        tap.aspiration   = 0.0;
        if (wasSilent) {
            EmitFrom(Ms(10), tap.Silenced(), tap);
            used += Ms(10);
        }
        Emit(Ms(spec.noiseMs), tap);
        used += Ms(spec.noiseMs);
        Emit(Ms(spec.transitionMs), vowelParams);
        used += Ms(spec.transitionMs);
        break;
    }

    case Manner::Glide: {
        // 「い」「う」に近い形から母音へゆっくり移る
        SpeechParams glide = WithLocus(vowelParams, locus, 0.0);
        if (spec.place == Place::Labial) {
            glide.formant[0] = 300.0;
            glide.formant[1] = 750.0;
        }
        glide.voicing = 0.85;
        if (wasSilent) {
            EmitFrom(Ms(20), glide.Silenced(), glide);
        } else {
            Emit(Ms(25), glide);
        }
        Emit(Ms(spec.noiseMs), glide);
        Emit(Ms(spec.transitionMs), vowelParams);
        used += Ms(spec.noiseMs + spec.transitionMs) + (wasSilent ? Ms(20) : Ms(25));
        break;
    }
    }
    return used;
}

double SpeechPlanner::PlanVowelTail(size_t index, const VowelTarget& vowel, const SpeechParams& vowelParams, bool emit) {
    const bool hasNext = index + 1 < moras_->size();
    const Mora* next   = hasNext ? &(*moras_)[index + 1] : nullptr;

    // 文末・区切り前はフェードアウト
    if (!next || next->kind == Mora::Kind::Pause) {
        if (emit) {
            Emit(Ms(60), vowelParams.Silenced());
        }
        return Ms(60);
    }

    // 「っ」の前: 次の次の子音に向けて口を閉じる
    if (next->kind == Mora::Kind::Geminate) {
        SpeechParams target = vowelParams.Silenced();
        if (index + 2 < moras_->size()) {
            if (const ConsonantSpec* spec = table_->FindConsonant((*moras_)[index + 2].consonant)) {
                target = WithLocus(vowelParams, table_->GetLocus(spec->place, vowel), 0.5).Silenced();
            }
        }
        if (emit) {
            Emit(Ms(35), target);
        }
        return Ms(35);
    }

    if (next->kind != Mora::Kind::Normal) {
        return 0.0;
    }
    const ConsonantSpec* spec = table_->FindConsonant(next->consonant);
    if (!spec) {
        return 0.0; // 母音→母音は次のモーラ側で遷移する
    }

    // 次の子音の調音位置に向けてフォルマントを寄せる(母音→子音の遷移)
    SpeechParams target = vowelParams;
    double duration     = Ms(25);
    switch (spec->manner) {
    case Manner::Stop:
    case Manner::Fricative:
    case Manner::Affricate:
        target         = WithLocus(vowelParams, table_->GetLocus(spec->place, vowel), 0.5);
        target.voicing = (std::min)(vowelParams.voicing, spec->isVoiced ? 0.6 : 0.25);
        duration       = Ms(25);
        break;
    case Manner::Aspirate:
        target.voicing = (std::min)(vowelParams.voicing, 0.5);
        break;
    case Manner::Nasal:
    case Manner::Flap:
    case Manner::Glide:
        target         = WithLocus(vowelParams, table_->GetLocus(spec->place, vowel), 0.6);
        target.voicing = (std::min)(vowelParams.voicing, 0.85);
        break;
    }
    if (emit) {
        Emit(duration, target);
    }
    return duration;
}

///============================================================
/// パラメータ作成
///============================================================
SpeechParams SpeechPlanner::MakeVowel(const VowelTarget& vowel, bool isDevoiced) const {
    SpeechParams p;
    p.formant    = vowel.formant;
    p.bandwidth  = vowel.bandwidth;
    p.voicing    = isDevoiced ? 0.0 : 1.0;
    p.aspiration = isDevoiced ? 0.3 : param_.breathiness;
    p.nasalZero  = 270.0;
    return p;
}

SpeechParams SpeechPlanner::MakeNasalMurmur(double zero, double f2) const {
    SpeechParams p;
    p.formant   = {250.0, std::clamp(f2, 800.0, 2200.0), 2300.0, 3400.0, 4500.0};
    p.bandwidth = {100.0, 300.0, 400.0, 300.0, 300.0};
    p.voicing   = 0.6;
    p.nasalZero = zero;
    return p;
}

SpeechParams SpeechPlanner::WithLocus(const SpeechParams& base, const PhonemeTable::Locus& locus, double ratio) const {
    SpeechParams p = base;
    for (size_t i = 0; i < locus.size(); ++i) {
        p.formant[i] = locus[i] + (base.formant[i] - locus[i]) * ratio;
    }
    return p;
}

void SpeechPlanner::ApplyNoiseShape(const ConsonantSpec& spec, const VowelTarget& vowel, SpeechParams& params) const {
    if (spec.noiseFrequency[0] > 0.0) {
        params.noiseFrequency = spec.noiseFrequency;
    } else {
        // 軟口蓋の破裂は後続母音のF2付近にピーク
        const double f        = std::clamp(vowel.formant[1] * 1.15, 1500.0, 3200.0);
        params.noiseFrequency = {f, f * 1.8};
    }
    params.noiseBandwidth = spec.noiseBandwidth;
}

bool SpeechPlanner::IsDevoiced(size_t index) const {
    if (!param_.enableDevoicing) {
        return false;
    }
    const Mora& mora = (*moras_)[index];
    if (mora.isQuestion || (mora.vowel != 'i' && mora.vowel != 'u') || !IsVoicelessConsonant(mora.consonant)) {
        return false;
    }
    // 無声子音に挟まれる / 文末・区切りの前
    if (index + 1 >= moras_->size()) {
        return true;
    }
    const Mora& next = (*moras_)[index + 1];
    switch (next.kind) {
    case Mora::Kind::Pause:
    case Mora::Kind::Geminate:
        return true;
    case Mora::Kind::Normal:
        return IsVoicelessConsonant(next.consonant);
    default:
        return false;
    }
}

bool SpeechPlanner::IsVoicelessConsonant(const std::string& consonant) const {
    return consonant == "k" || consonant == "s" || consonant == "sh" || consonant == "t" || consonant == "ch" || consonant == "ts"
        || consonant == "h" || consonant == "f" || consonant == "p";
}

double SpeechPlanner::Ms(double ms) const {
    return ms * 0.001 / (std::max)(param_.speed, 0.1);
}

///============================================================
/// 出力
///============================================================
void SpeechPlanner::Emit(double seconds, const SpeechParams& target) {
    EmitFrom(seconds, current_, target);
}

void SpeechPlanner::EmitFrom(double seconds, const SpeechParams& start, const SpeechParams& target) {
    if (seconds <= 0.0) {
        current_    = target;
        current_.f0 = prosody_->PitchAt(position_);
        return;
    }
    SpeechSegment segment;
    segment.duration = seconds;
    segment.start    = start;
    segment.end      = target;
    segment.start.f0 = prosody_->PitchAt(position_);
    position_ += seconds / moraLength_;
    segment.end.f0 = prosody_->PitchAt(position_);

    segments_.push_back(segment);
    current_ = segment.end;
}
