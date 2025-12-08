#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include "Easing/Easing.h"
#include "MovieLine.h"
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// 目的カットイン表示演出クラス
/// </summary>
class IntroPurposeCutIn : public BaseGameIntro {
public:
    enum class Step {
        SideAppearWait,
        SideAppear,
        CenterAppearWait,
        CenterAppear,
        CloseWait,
        Close,
        FinishWait,
        Finish,
    };

    enum SpritePos {
        LEFT,
        RIGHT,
        CENTER,
        COUNT,
    };

    struct SpriteVariable {
        std::array<float, 2> sideAppearPosX;
        std::array<float, 2> sideBackPosX;
        float appearScaleY;
        float closeScaleY;
        Vector2 centerScale;
        std::array<std::unique_ptr<Easing<float>>, 2> sideAppearEase;
        std::array<std::unique_ptr<Easing<float>>, 2> sideBackEase;
        std::unique_ptr<Easing<Vector2>> centerAppearEase;
        std::unique_ptr<Easing<float>> appearScaleEaseY;
        std::unique_ptr<Easing<float>> closeScaleEaseY;
        bool isBackSideUI = false;
    };

public:
    IntroPurposeCutIn()           = default;
    ~IntroPurposeCutIn() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="name">イントロ名</param>
    void Init(const std::string& name) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(float playSpeed) override;

    // Param Edit
    void AdjustParam() override;       //< パラメータ調整
    void RegisterParams() override;        //< パラメータバインド
    void AdjustUniqueParam() override; //< 固有パラメータ調整

private:
    // Step function
    void SideAppearWait();   //< サイド出現待機
    void SideAppear();       //< サイド出現
    void CenterAppearWait(); //< 中央出現待機
    void CenterAppear();     //< 中央出現
    void CloseWait();        //< 終了待機
    void Close();            //< 終了
    void FinishWait();       //< 完了待機
    void Finish();           //< 完了
    void EasingInit();       //< イージング初期化

    /// <summary>
    /// ステップ処理
    /// </summary>
    /// <param name="limitTime">制限時間</param>
    /// <param name="nextStep">次のステップ</param>
    /// <param name="enableEnemySpawn">敵生成有効化</param>
    /// <returns>ステップ完了でtrue</returns>
    bool ProcessStep(float limitTime, const Step& nextStep, const bool& enableEnemySpawn = false);

private:
    static void (IntroPurposeCutIn::* spFuncTable_[])();

private:
    float sideAppearWaitTime_;
    float closeWaitTime_;
    float centerAppearWaitTime_;
    float finishWaitTime_;
    std::unique_ptr<Sprite> backLineSprite_;
    std::array<std::unique_ptr<Sprite>, static_cast<size_t>(SpritePos::COUNT)> sprites_;
    SpriteVariable spriteVariable_;
    Step step_ = Step::SideAppearWait;
};