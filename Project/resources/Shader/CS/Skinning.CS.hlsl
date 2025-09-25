

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ShadowTransformBuffer> gShadowTransformBuffer : register(b1);

StructuredBuffer<Well> gMatrixPalette : register(t0);

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
}