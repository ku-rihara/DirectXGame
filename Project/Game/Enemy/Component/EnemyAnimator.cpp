#include "EnemyAnimator.h"
#include <algorithm>

void EnemyAnimator::Init(const std::string& modelFolder) {
    modelFolder_          = modelFolder;
    chaseAnimeState_      = ChaseAnimationState::NONE;
    isPreDashFinished_    = false;
    damageReactionAnimations_.clear();
}

void EnemyAnimator::SetAnimationName(AnimationType type, const std::string& name) {

    // Waitアニメーションが最初にセットされるので、オブジェクトの生成も行う
    if (type == AnimationType::Wait) {
        if (!objAnimation_) {
            objAnimation_.reset(KetaEngine::Object3DAnimation::CreateModel(modelFolder_ + name + ".gltf"));
        }
        // Waitアニメーション名のセット
        animationNames_[static_cast<size_t>(type)] = name;
        return;
    }

    // 使用するアニメーションの追加
    if (animationNames_[static_cast<size_t>(type)].empty()) {
        objAnimation_->Add(modelFolder_ + name + ".gltf");
    }

    // アニメーション名のセット
    animationNames_[static_cast<size_t>(type)] = name;
}

void EnemyAnimator::AddDamageReactionAnimation(const std::string& name, bool isLoop) {
    // ダメージリアクションのアニメーション追加
    objAnimation_->Add(modelFolder_ + name + ".gltf");

    // 再スポーン時の重複登録を防ぐ
    for (auto& info : damageReactionAnimations_) {
        if (info.name == name) {
            info.isLoop = isLoop;
            return;
        }
    }
    damageReactionAnimations_.push_back({name, isLoop});
}

void EnemyAnimator::AddAnimationFile(const std::string& relativeName) {
    // アニメーションファイルを追加
    objAnimation_->Add(modelFolder_ + relativeName + ".gltf");
}

void EnemyAnimator::PlayAnimation(AnimationType type, bool isLoop) {
    if (!objAnimation_) {
        return;
    }

    // タイプからアニメーション名の取得
    const std::string& animeName = GetAnimationName(type);
    if (animeName.empty()) {
        return;
    }

    // アニメーションの再生
    objAnimation_->ChangeAnimation(animeName);
    objAnimation_->SetLoop(isLoop);
}

bool EnemyAnimator::PlayAnimationByName(const std::string& animationName, bool isLoop) {
    if (!objAnimation_ || animationName.empty()) {
        return false;
    }

    // アニメーション名が存在するか確認
    auto animeNames = objAnimation_->GetAnimationNames();
    auto it         = std::find(animeNames.begin(), animeNames.end(), animationName);

    if (it != animeNames.end()) {
        // アニメーションが存在すれば再生
        objAnimation_->ChangeAnimation(animationName);
        objAnimation_->SetLoop(isLoop);
        return true;
    }

    return false;
}

void EnemyAnimator::UpdateChaseAnimation([[maybe_unused]] float deltaTime) {
    if (!objAnimation_) {
        return;
    }

    if (chaseAnimeState_ == ChaseAnimationState::PRE_DASH && isPreDashFinished_) {
        // ダッシュアニメーションに切り替え
        chaseAnimeState_ = ChaseAnimationState::DASHING;
        objAnimation_->ChangeAnimation(GetAnimationName(AnimationType::Dash));
        objAnimation_->SetLoop(true);
    }
}

void EnemyAnimator::ResetToWaitAnimation() {
    if (!objAnimation_) {
        return;
    }

    // 待機アニメーションにリセット
    chaseAnimeState_   = ChaseAnimationState::NONE;
    isPreDashFinished_ = false;
    objAnimation_->ChangeAnimation(GetAnimationName(AnimationType::Wait));
    objAnimation_->SetLoop(true);
}

void EnemyAnimator::RotateInit() {
    if (objAnimation_) {
        objAnimation_->transform_.rotation_ = Vector3::ZeroVector();
    }
}

void EnemyAnimator::ClearAllCallbacks() {
    if (objAnimation_) {
        objAnimation_->ClearAllAnimationEndCallbacks();
    }
}

// Getter-------------------------------------------------------------------------

Vector3 EnemyAnimator::GetBodyRotation() const {
    return objAnimation_->transform_.rotation_;
}

std::vector<std::string> EnemyAnimator::GetAnimationNames() const {
    if (objAnimation_) {
        return objAnimation_->GetAnimationNames();
    }
    return {};
}

const std::string& EnemyAnimator::GetAnimationName(AnimationType type) const {
    return animationNames_[static_cast<size_t>(type)];
}

std::vector<std::string> EnemyAnimator::GetDamageReactionAnimationNames() const {
    std::vector<std::string> names;
    names.reserve(damageReactionAnimations_.size());
    for (const auto& info : damageReactionAnimations_) {
        names.push_back(info.name);
    }
    return names;
}

bool EnemyAnimator::GetDamageReactionAnimationIsLoop(const std::string& name) const {
    for (const auto& info : damageReactionAnimations_) {
        if (info.name == name) {
            return info.isLoop;
        }
    }
    return false;
}

/// Setter-------------------------------------------------------------------------

void EnemyAnimator::SetAnimationActive(bool active) {
    if (objAnimation_) {
        objAnimation_->SetIsActive(active);
    }
}

void EnemyAnimator::SetBodyRotate(const Vector3& rotate) {
    objAnimation_->transform_.rotation_ = rotate;
}

void EnemyAnimator::SetBodyColor(const Vector4& color) {
    if (objAnimation_) {
        objAnimation_->GetModelMaterial()->GetMaterialData()->color = color;
    }
}
