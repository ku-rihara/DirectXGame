#pragma once
#include <vector>
#include <string>
#include "Vector3.h"
#include <json.hpp>

class EmitControlPosManager {
private:
    using json = nlohmann::json; // 別名を定義
private:

    ///=====================================================
    /// private variants
    ///=====================================================

    std::vector<Vector3> movePosies_;  // 移動座標リスト
    std::string filePath_;         // 座標データの保存先
    Vector3 tempAddPosition_;      // ImGuiで座標を一時入力する変数

    const std::string  dyrectrypath_ = "./Resources/GlobalParameter/EmitControlPos/";

public:
    EmitControlPosManager();

    ///=====================================================
  /// pbulic method
  ///=====================================================
  
    // 座標管理
    void AddPosition(const Vector3& position);
    void RemovePosition(size_t index);
  

    // ファイル操作
    void SaveToFile(const std::string&filename);
    void LoadFromFile(const std::string& filename);

    // ImGui用UI
    void ImGuiUpdate(const std::string& filename);

    ///=====================================================
   /// getter method
   ///=====================================================
    const std::vector<Vector3>& GetPositions() const;
};
