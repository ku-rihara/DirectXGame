#pragma once

#include "IAudioFilter.h"
#include <memory>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 複数フィルターの並列接続(各出力の和)
/// </summary>
class AudioFilterBank : public IAudioFilter {
public:
    void Reset() override;
    double ProcessSample(double x) override;

    IAudioFilter* Add(std::unique_ptr<IAudioFilter> filter);
    void ClearFilters() { filters_.clear(); }

private:
    std::vector<std::unique_ptr<IAudioFilter>> filters_;

public:
    size_t GetFilterCount() const { return filters_.size(); }
    IAudioFilter* GetFilter(size_t index) const { return filters_[index].get(); }
};

}; // KetaEngine
