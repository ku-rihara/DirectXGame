#include "../Particle.hlsli"
#include"../../Library/PerFrame.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<int> gFreeList : register(u2);
ConstantBuffer<PerFrame> gPerFrame : register(b0);

// 速度ベクトルから回転角度を計算
float CalculateZRotationFromVelocity(float3 velocity)
{
    float velocityLength = length(velocity.xy);
    if (velocityLength < 0.001f)
    {
        return 0.0f;
    }
    
    // XY平面での角度を計算
    return atan2(velocity.y, velocity.x);
}

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        if (gParticles[particleIndex].color.a != 0)
        {
            // 位置更新
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity * gPerFrame.deltaTime;
            
            // スピン回転を加算
            float3 spinRotation = gParticles[particleIndex].rotateSpeed * gPerFrame.deltaTime;
            
            // 進行方向の回転は速度から直接計算
            float velocityRotationZ = CalculateZRotationFromVelocity(gParticles[particleIndex].velocity);
            
            // X, Y軸はスピン回転を適用
            gParticles[particleIndex].rotate.x += spinRotation.x;
            gParticles[particleIndex].rotate.y += spinRotation.y;
            
            // Z軸は進行方向 + スピン回転
            gParticles[particleIndex].rotate.z = velocityRotationZ + spinRotation.z;
            
            // 時間更新
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
            
            if (gParticles[particleIndex].color.a == 0)
            {
                gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
                int freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                
                if ((freeListIndex + 1) < kMaxParticles)
                {
                    gFreeList[freeListIndex + 1] = particleIndex;
                }
                else
                {
                    InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                }
            }
        }
    }
}