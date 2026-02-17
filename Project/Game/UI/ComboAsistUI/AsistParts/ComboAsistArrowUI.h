#pragma once
#include "BaseComboAsistUI.h"
#include <cstdint>

/// <summary>
/// コンボアシストでの矢印UIクラス
/// </summary>
class ComboAsistArrowUI : public BaseComboAsistUI {
public:
    ComboAsistArrowUI()           = default;
    ~ComboAsistArrowUI() override = default;

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="fromCol">始点の列</param>
    /// <param name="fromRow">始点の行</param>
    /// <param name="toCol">終点の列</param>
    /// <param name="toRow">終点の行</param>
    /// <param name="layout">レイアウト情報</param>
    void Init(int32_t fromCol, int32_t fromRow, int32_t toCol, int32_t toRow, const LayoutParam& layout);

    void Update() override;
    void ApplyLayout() override;

    /// <summary>
    /// 列番号をシフトする
    /// </summary>
    void ShiftColumns(int32_t delta);

    // getter
    int32_t GetFromCol() const { return fromCol_; }
    int32_t GetFromRow() const { return fromRow_; }
    int32_t GetToCol() const { return toCol_; }
    int32_t GetToRow() const { return toRow_; }

private:
    int32_t fromCol_ = 0;
    int32_t fromRow_ = 0;
    int32_t toCol_   = 0;
    int32_t toRow_   = 0;
};
