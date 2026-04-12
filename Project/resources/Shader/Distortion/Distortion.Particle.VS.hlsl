#include "Distortion.hlsli"

struct VSInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal   : NORMAL0;
};

struct ParticleForGPU {
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
    float4x4 UVTransform;
    float4   color;
    uint     isFlipX;
    uint     isFlipY;
    float    dissolveThreshold;
};
StructuredBuffer<ParticleForGPU> gParticle : register(t0);

VSOutput main(VSInput input, uint instanceID : SV_InstanceID) {
    VSOutput output;

    output.position = mul(input.position, gParticle[instanceID].WVP);

    // Flip 処理
    float2 texcoord = input.texcoord;
    if (gParticle[instanceID].isFlipX != 0) {
        texcoord.x = 1.0f - texcoord.x;
    }
    if (gParticle[instanceID].isFlipY != 0) {
        texcoord.y = 1.0f - texcoord.y;
    }

    // UVTransform を適用
    float4 transformedUV = mul(float4(texcoord, 0.0f, 1.0f), gParticle[instanceID].UVTransform);
    output.texcoord = transformedUV.xy;
    output.color    = gParticle[instanceID].color;
    return output;
}
