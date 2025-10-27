#include"../Particle.hlsli"
#include"../../Library/Random.hlsli"
#include"../../Library/PerFrame.hlsli"


float3 RandomRange3d(RandomGenerator generator, float3 minValue, float3 maxValue)
{
    float3 t = generator.Generate3d();
    return lerp(minValue, maxValue, t);
}

float RandomRange1d(RandomGenerator generator, float minValue, float maxValue)
{
    float t = generator.Generate1d();
    return lerp(minValue, maxValue, t);
}

float4 RandomRange4d(RandomGenerator generator, float4 minValue, float4 maxValue)
{
    float4 t = float4(
        generator.Generate1d(),
        generator.Generate1d(),
        generator.Generate1d(),
        generator.Generate1d()
    );
    return lerp(minValue, maxValue, t);
}


ConstantBuffer<Emitter> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
ConstantBuffer<EmitParameter> gEmitParam : register(b2);

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<int> gFreeList : register(u2);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
    
    if (gEmitter.emit != 0)
    {
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            
            if (0 <= freeListIndex && freeListIndex < kMaxParticles)
            {
                int particleIndex = gFreeList[freeListIndex];
                
                // Emit時のパラメータを設定
                
                // scale
                gParticles[particleIndex].scale = RandomRange3d(generator,
                    gEmitParam.scaleMin, gEmitParam.scaleMax);
                
                // rotation
                gParticles[particleIndex].rotate = RandomRange3d(generator,
                    gEmitParam.rotationMin, gEmitParam.rotationMax);
                
                // roationSpeed
                gParticles[particleIndex].rotateSpeed = RandomRange3d(generator,
                    gEmitParam.rotationSpeedMin, gEmitParam.rotationSpeedMax);
                
                // translate
                float3 randomOffset = RandomRange3d(generator,
                    gEmitParam.translateMin, gEmitParam.translateMax);
                gParticles[particleIndex].translate = gEmitter.translate +
                    generator.Generate3d() * gEmitter.radius + randomOffset;
                
                // velosity
                gParticles[particleIndex].velocity = RandomRange3d(generator,
                    gEmitParam.velocityMin, gEmitParam.velocityMax);
                
                // color
                gParticles[particleIndex].color = RandomRange4d(generator,
                    gEmitParam.colorMin, gEmitParam.colorMax);
                
                // lifeTime
                gParticles[particleIndex].lifeTime = RandomRange1d(generator,
                    gEmitParam.lifeTimeMin, gEmitParam.lifeTimeMax);
                gParticles[particleIndex].currentTime = 0.0f;
               
            }
            else
            {
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}