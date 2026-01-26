#pragma once
#include "3d/Object3d/Object3d.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "RailData.h"
#include <memory>
#include <string>

namespace KetaEngine {

class Line3D;

/// <summary>
/// レールエディター
/// </summary>
class RailEditor : public BaseEffectEditor<RailData> {
public:
    RailEditor();
    ~RailEditor();

    // 初期化、更新
    void Init(const std::string& editorName) override;
    void Update(float speedRate = 1.0f) override;
  
private:
    std::unique_ptr<RailData> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;

    void DebugLineSet();

private:
    std::unique_ptr<Object3d> preViewObj_;
    std::unique_ptr<Line3D> debugLine3D_;
    const std::string folderName_     = "RailEditor/";
    
    Vector3 basePos_                  = Vector3::ZeroVector();
    bool isPreViewDraw_               = false;

public:
    int GetRailCount() const;

    std::string GetFolderName() const override;
};

}; // KetaEngine