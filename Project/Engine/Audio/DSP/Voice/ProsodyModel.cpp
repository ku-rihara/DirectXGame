#include "ProsodyModel.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>

void ProsodyModel::Setup(const std::vector<Mora>& moras) {
    const size_t count = moras.size();
    targets_.assign(count + 1, param_.basePitch);
    if (count == 0) {
        return;
    }

    // 句(Pauseで区切られた範囲)ごとに高さを決める
    size_t phraseStart = 0;
    int phraseIndex    = 0;
    while (phraseStart < count) {
        size_t phraseEnd = phraseStart;
        while (phraseEnd < count && moras[phraseEnd].kind != Mora::Kind::Pause) {
            ++phraseEnd;
        }

        const size_t length    = phraseEnd - phraseStart;
        const double phraseTop = 1.0 - param_.phraseDrop * phraseIndex;
        for (size_t k = 0; k < length; ++k) {
            double factor;
            if (k == 0 && length > 1) {
                // 句頭は低く始まる
                factor = phraseTop - param_.accentRise * 0.6;
            } else {
                // 2拍目で高くなり、徐々に下がる
                const double progress = (length > 2) ? static_cast<double>(k - 1) / static_cast<double>(length - 2) : 0.0;
                factor                = phraseTop + param_.accentRise * 0.4 - param_.declination * progress;
            }
            targets_[phraseStart + k] = param_.basePitch * factor;
        }

        // 句末: 次のモーラ境界の高さで下降/上昇を表現
        if (length > 0) {
            const size_t last  = phraseEnd - 1;
            const double lastF = targets_[last];
            targets_[phraseEnd] = moras[last].isQuestion ? lastF * (1.0 + param_.questionRise) : lastF * (1.0 - param_.finalFall);
        }

        // Pauseを飛ばして次の句へ(無音区間なので高さは使われない)
        phraseStart = phraseEnd + 1;
        if (length > 0) {
            ++phraseIndex;
        }
    }
}

double ProsodyModel::PitchAt(double position) const {
    if (targets_.empty()) {
        return param_.basePitch;
    }
    const double maxPos = static_cast<double>(targets_.size() - 1);
    position            = std::clamp(position, 0.0, maxPos);

    const size_t index = static_cast<size_t>(std::floor(position));
    if (index + 1 >= targets_.size()) {
        return targets_.back();
    }
    // なめらかに補間
    double t = position - static_cast<double>(index);
    t        = t * t * (3.0 - 2.0 * t);
    return targets_[index] + (targets_[index + 1] - targets_[index]) * t;
}
