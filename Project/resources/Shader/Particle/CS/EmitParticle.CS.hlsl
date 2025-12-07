#include"../Particle.hlsli"
#include"../../Library/Random.hlsli"
#include"../../Library/PerFrame.hlsli"

float3 RandomRange3d(inout RandomGenerator generator, float3 minValue, float3 maxValue)
{
    float3 t = generator.Generate3d();
    return lerp(minValue, maxValue, t);
}

float RandomRange1d(inout RandomGenerator generator, float minValue, float maxValue)
{
    float t = generator.Generate1d();
    return lerp(minValue, maxValue, t);
}

float4 RandomRange4d(inout RandomGenerator generator, float4 minValue, float4 maxValue)
{
    float4 t = float4(
        generator.Generate1d(),
        generator.Generate1d(),
        generator.Generate1d(),
        generator.Generate1d()
    );
    return lerp(minValue, maxValue, t);
}

// 定数バッファ定義
ConstantBuffer<Emitter> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
ConstantBuffer<EmitTransformParams> gTransformParams : register(b2);
ConstantBuffer<EmitPhysicsParams> gPhysicsParams : register(b3);
ConstantBuffer<EmitAppearanceParams> gAppearanceParams : register(b4);

// UAVバッファ定義
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
                
                // ----------------------------- Transform Parameters ----------------------------- //
                
                // Scale
                gParticles[particleIndex].scale = RandomRange3d(generator,
                    gTransformParams.scaleMin, gTransformParams.scaleMax);
                
                // Rotation (初期回転)
                gParticles[particleIndex].rotate = RandomRange3d(generator,
                    gTransformParams.rotationMin, gTransformParams.rotationMax);
                
                // Position 
                float3 randomOffset = RandomRange3d(generator,
                    gTransformParams.translateMin, gTransformParams.translateMax);
                gParticles[particleIndex].translate = gEmitter.translate + randomOffset;
                
                // ----------------------------- Physics Parameters ----------------------------- //
                
                // Velocity (初速度)
                gParticles[particleIndex].velocity = RandomRange3d(generator,
                    gPhysicsParams.velocityMin, gPhysicsParams.velocityMax);
                
                // Rotation Speed (回転速度)
                gParticles[particleIndex].rotateSpeed = RandomRange3d(generator,
                    gPhysicsParams.rotationSpeedMin, gPhysicsParams.rotationSpeedMax);
                
                // ----------------------------- Appearance Parameters ----------------------------- //
                
                // Color
                gParticles[particleIndex].color = RandomRange4d(generator,
                    gAppearanceParams.colorMin, gAppearanceParams.colorMax);
                
                // LifeTime
                gParticles[particleIndex].lifeTime = RandomRange1d(generator,
                    gAppearanceParams.lifeTimeMin, gAppearanceParams.lifeTimeMax);
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