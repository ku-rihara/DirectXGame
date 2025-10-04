
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
RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (gEmitter.emit != 0)
    {
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {   
            //カウント分放出
            gParticles[countIndex].scale = float3(0.3f, 0.3f, 0.3f);
            gParticles[countIndex].translate = float3(0.3f, 0.3f, 0.3f);
            gParticles[countIndex].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}