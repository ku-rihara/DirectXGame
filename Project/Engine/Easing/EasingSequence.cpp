#include "EasingSequence.h"

template <typename T>
void EasingSequence::AddStep(const std::string& name, T* adaptValue) {
    auto step = std::make_unique<EasingStep<T>>();
    step->ApplyFromJson(name);
    step->SetAdaptValue(adaptValue);
    step->Update(0.0f);

    // Add時に startValue を反映
    if (adaptValue) {
        *adaptValue = step->GetValue();
    }

    step->SetOnWaitEndCallback([this, adaptValue]() {
        if (steps_.empty()) {
            return;
        }

        // 次のステップへ
        if (currentStep_ + 1 < steps_.size()) {
            ++currentStep_;
            steps_[currentStep_]->Reset();

            if (adaptValue) {
                if (auto* typed = dynamic_cast<EasingStep<T>*>(steps_[currentStep_].get())) {
                    *adaptValue = typed->GetValue();
                }
            }
        }
        // 最後のステップの場合
        else {
            if (loop_) {
                currentStep_ = 0;
                steps_[currentStep_]->Reset();

                if (adaptValue) {
                    if (auto* typed = dynamic_cast<EasingStep<T>*>(steps_[currentStep_].get())) {
                        *adaptValue = typed->GetValue();
                    }
                }
            }

            // 全ステップ終了コールバック
            if (onAllFinishCallback_) {
                onAllFinishCallback_();
            }
        }
    });

    steps_.emplace_back(std::move(step));
}

template <typename T>
void EasingSequence::AddStep(std::unique_ptr<Easing<T>> easing) {
    steps_.emplace_back(std::make_unique<EasingStep<T>>(std::move(easing)));
}

void EasingSequence::Reset() {
    currentStep_ = 0;
    for (auto& step : steps_) {
        step->Reset();
    }
}

void EasingSequence::Update(const float& deltaTime) {
    if (steps_.empty()) {
        return;
    }

    if (currentStep_ >= steps_.size()) {
        return;
    }

    auto* step = steps_[currentStep_].get();
    step->Update(deltaTime);
}

template <typename T>
void EasingSequence::SetBaseValue(const T& value) {
    for (auto& step : steps_) {
        if (auto* typed = dynamic_cast<EasingStep<T>*>(step.get())) {
            typed->SetBaseValue(value);
        }
    }
}

const IEasingStep* EasingSequence::GetCurrentStep() const {
    if (currentStep_ < steps_.size())
        return steps_[currentStep_].get();
    return nullptr;
}


bool EasingSequence::IsAllFinished() const {
    if (steps_.empty()) {
        return true;
    }

    // ループが有効なら常にFalse
    if (loop_) {
        return false;
    }

    // 最後のステップかつ終了
    return currentStep_ >= steps_.size() - 1 && steps_[currentStep_]->IsFinished();
}

bool EasingSequence::IsAllPlaying() const {

    for (const auto& step : steps_) {
        if (step->IsPlaying()) {
            return true;
        }
    }
    return false;
}

// 明示的インスタンス化
template void EasingSequence::AddStep<float>(const std::string&, float*);
template void EasingSequence::AddStep<Vector2>(const std::string&, Vector2*);
template void EasingSequence::AddStep<Vector3>(const std::string&, Vector3*);
template void EasingSequence::AddStep<float>(std::unique_ptr<Easing<float>>);
template void EasingSequence::AddStep<Vector2>(std::unique_ptr<Easing<Vector2>>);
template void EasingSequence::AddStep<Vector3>(std::unique_ptr<Easing<Vector3>>);
template void EasingSequence::SetBaseValue<float>(const float&);
template void EasingSequence::SetBaseValue<Vector2>(const Vector2&);
template void EasingSequence::SetBaseValue<Vector3>(const Vector3&);