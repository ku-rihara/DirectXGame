//// ComboCreateEditor.cpp
//#include "ComboCreateEditor.h"
//#include <fstream>
//#include <json.hpp>
//
//void ComboCreateEditor::Edit() {
//    ImGui::Begin("Combo Create Editor");
//
//    // コンボリスト
//    ImGui::Text("Combo List");
//    for (int i = 0; i < combos_.size(); ++i) {
//        if (ImGui::Selectable(combos_[i].name.c_str(), selectedCombo_ == i)) {
//            selectedCombo_ = i;
//        }
//    }
//    if (ImGui::Button("Add Combo")) {
//        combos_.push_back({});
//        selectedCombo_ = combos_.size() - 1;
//    }
//
//    ImGui::Separator();
//
//    //// 選択中のコンボ編集
//    //if (!combos_.empty()) {
//    //    ComboAttackParameter& param = combos_[selectedCombo_];
//    //    ImGui::InputText("Name", &param.name);
//    //    ImGui::InputText("Start Condition", &param.startCondition);
//    //    ImGui::InputText("End Condition", &param.endCondition);
//    //    ImGui::InputText("Motion Type", &param.motionType);
//    //    ImGui::InputText("Easing Preset", &param.easingPreset);
//    //    ImGui::DragFloat("Easing Time", &param.easingTime, 0.01f, 0.0f, 10.0f);
//    //    ImGui::InputText("Collision Type", &param.collisionType);
//    //    ImGui::DragFloat3("Collision Size", &param.collisionSize.x, 0.1f);
//    //    ImGui::DragFloat3("Collision Offset", &param.collisionOffset.x, 0.1f);
//    //}
//
//    if (ImGui::Button("Save")) {
//        Save("combo_attack.json");
//    }
//    ImGui::SameLine();
//    if (ImGui::Button("Load")) {
//        Load("combo_attack.json");
//    }
//
//    ImGui::End();
//}
//
//
////void ComboCreateEditor::Save(const std::string& path) {
////  /*  nlohmann::json j = combos_;
////    std::ofstream ofs(path);
////    ofs << j.dump(4);*/
////}
////void ComboCreateEditor::Load(const std::string& path) {
////   /* std::ifstream ifs(path);
////    if (!ifs)
////        return;
////    nlohmann::json j;
////    ifs >> j;
////    combos_ = j.get<std::vector<ComboAttackParameter>>();*/
////}