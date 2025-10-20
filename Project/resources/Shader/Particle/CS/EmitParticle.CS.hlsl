#include"../Particle.hlsli"
#include"../../Library/Random.hlsli"
#include"../../Library/PerFrame.hlsli"

struct EmitterSphere
{
    float3 translate;
    float radius;
    uint count;
    float frequency;
    float frequencyTime;
    uint emit;
};


ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gCounter : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
    
    if (gEmitter.emit != 0)
    {
        
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            
            int particleIndex;
            InterlockedAdd(gCounter[0], 1, particleIndex);
            if (particleIndex >= kMaxParticles)
            {
                //上限到達したら終了
                return;
            }
            
            //カウント分放出
            gParticles[countIndex].scale = generator.Generate3d();
            gParticles[countIndex].translate = generator.Generate3d();
            gParticles[countIndex].color.rgb = generator.Generate3d();
            gParticles[countIndex].color.w = 1.0f;

        }
    }
}