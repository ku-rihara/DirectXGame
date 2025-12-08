
static const uint kMaxParticles = 1024;

struct Emitter
{
    float3 translate;
    uint count;
    float frequency;
    float frequencyTime;
    uint emit;
};

struct Particle
{
    float3 translate;
    float3 scale;
    float3 rotate;
    float3 rotateSpeed;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
 
};

struct EmitTransformParams
{
    float3 scaleMin;
    float3 scaleMax;
    float3 rotationMin;
    float3 rotationMax;
    float3 translateMin;
    float3 translateMax;
};

struct EmitPhysicsParams
{
    float3 velocityMin;
    float3 velocityMax;
    float3 rotationSpeedMin;
    float3 rotationSpeedMax;
   
};

struct EmitAppearanceParams
{
    float4 colorMin;
    float4 colorMax;
    float lifeTimeMin;
    float lifeTimeMax;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
