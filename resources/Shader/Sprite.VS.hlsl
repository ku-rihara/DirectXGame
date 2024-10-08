#include"Particle.hlsli"
//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticle[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceID].WorldInverseTranspose));
    
    return output;
}