#include <chrono>

/// <summary>
/// フレームクラス
/// </summary>
class Frame {
private:
    /// ========================================================
    /// 静的メンバ変数
    /// ========================================================

    static std::chrono::high_resolution_clock::time_point lastTime_;
    static float deltaTime_;
public:
    /// ========================================================
    /// 静的メンバ関数
    /// ========================================================
   
    static void Init();
    static void Update();
    static float DeltaTime();
};
