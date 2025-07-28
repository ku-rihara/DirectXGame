
struct ShadowPixelInput
{
    float4 position : SV_POSITION;
    float depth : TEXCOORD0;
};

struct ShadowPixelOutput
{
    float depth : SV_TARGET0;
};

ShadowPixelOutput main(ShadowPixelInput input)
{
    ShadowPixelOutput output;
    
    // デプス値をそのまま出力
    output.depth = input.depth;
    
    return output;
}