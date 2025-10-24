#include "../Particle.hlsli"
#include"../../Library/PerFrame.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<int> gFreeList : register(u2);
ConstantBuffer<PerFrame> gPerFrame : register(b0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        //alpha0は死んでるとみなして更新しない
        if (gParticles[particleIndex].color.a != 0)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
            
            //alphaが0になったっらfree
            if (gParticles[particleIndex].color.a == 0)
            {
                //スケール0を入れてVertexShader出力で棄却されるようにする
                gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
                int freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                //最新のFreeListIndexの場所に死んだParticleのindexを設定する
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