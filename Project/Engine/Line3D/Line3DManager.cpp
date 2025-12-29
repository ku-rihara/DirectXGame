#include "Line3DManager.h"

using namespace KetaEngine;
#include "Dx/DirectXCommon.h"
#include "Line3D.h"
#include "Pipeline/PipelineManager.h"
#include <cstdlib>

bool Line3DManager::isDestroyed_ = false;

///============================================================
/// デストラクタ
///============================================================
Line3DManager::~Line3DManager() {
    isDestroyed_ = true;
    lines_.clear();
}

///============================================================
/// シングルトンインスタンス取得
///============================================================
Line3DManager* Line3DManager::GetInstance() {
    static Line3DManager instance;
    static bool isAlive = true;

    // アプリケーション終了処理中かチェック
    if (!isAlive) {
        return nullptr;
    }

    // 初回呼び出し時にAtexit登録
    static bool registered = false;
    if (!registered) {
        std::atexit([]() { isAlive = false; });
        registered = true;
    }

    return &instance;
}

///============================================================
/// Line3Dオブジェクト登録
///============================================================
void Line3DManager::RegisterLine(Line3D* line) {
    if (line != nullptr) {
        lines_.insert(line);
    }
}

///============================================================
/// Line3Dオブジェクト登録解除
///============================================================
void Line3DManager::UnregisterLine(Line3D* line) {
    if (line != nullptr) {
        lines_.erase(line);
    }
}

///============================================================
/// 全Line3Dオブジェクトの一括描画
///============================================================
void Line3DManager::DrawAll(const ViewProjection& viewProj) {
    viewProj;
#ifdef _DEBUG
    if (lines_.empty()) {
        return;
    }

    auto commandList = DirectXCommon::GetInstance()->GetCommandList();

    // Line3D用のパイプラインを一度だけ設定
    PipelineManager::GetInstance()->PreDraw(PipelineType::Line3D, commandList);

    // 登録された全てのLine3Dを描画
    auto linesCopy = lines_;
    for (Line3D* line : linesCopy) {
        if (line != nullptr && lines_.find(line) != lines_.end()) {
            line->Draw(viewProj);
        }
    }

    // Object3D用のパイプラインに戻す
    PipelineManager::GetInstance()->PreDraw(PipelineType::Object3D, commandList);
#endif // _DEBUG
}

///============================================================
/// 全Line3Dオブジェクトのリセット
///============================================================
void Line3DManager::ResetAll() {
    auto linesCopy = lines_;
    for (Line3D* line : linesCopy) {
        if (line != nullptr && lines_.find(line) != lines_.end()) {
            line->Reset();
        }
    }
}

///============================================================
/// 全Line3Dオブジェクトクリア
///============================================================
void Line3DManager::Clear() {
    lines_.clear();
}
