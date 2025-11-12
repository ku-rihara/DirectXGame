#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include "MovieLine.h"
#include <memory>

/// <summary>
/// イントロフィールド生成演出クラス
/// </summary>
class IntroSpawnField : public BaseGameIntro {
public:
    enum class Step {
        START,
        WAIT,
        OBJSPAWN,
        PLAYERSPAWN,
        FNISHWAIT,
        FINISH,
    };

public:
    IntroSpawnField()           = default;
    ~IntroSpawnField() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="name">イントロ名</param>
    void Init(const std::string& name) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(const float& playSpeed) override;

    // Param Edit
    void AdjustParam() override; //< パラメータ調整
    void RegisterParams() override; //< パラメータバインド
    void AdjustUniqueParam() override; //< 固有パラメータ調整

private:
    // Step function
    void Start();       //< 開始
    void Wait();        //< 待機
    void ObjSpawn();    //< オブジェクト生成
    void PlayerSpawn(); //< プレイヤー生成
    void FinishWait();  //< 終了待機
    void Finish();      //< 終了

    /// <summary>
    /// ステップ処理
    /// </summary>
    /// <param name="limitTime">制限時間</param>
    /// <param name="nextStep">次のステップ</param>
    /// <param name="enableEnemySpawn">敵生成有効化</param>
    /// <returns>ステップ完了でtrue</returns>
    bool ProcessStep(const float& limitTime, const Step& nextStep, const bool& enableEnemySpawn = false);

private:
    static void (IntroSpawnField::* spFuncTable_[])();

private:
    float waitTime_;
    float objSpawnTime_;
    float playerSpawnTime_;
    float finishWaitTime_;
    Step step_ = Step::START;

public:
    const Step& GetCurrentStep() const { return step_; }
};