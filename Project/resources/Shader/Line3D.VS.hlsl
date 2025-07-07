#include"Line3D.hlsli"

struct ViewProjection
{
    matrix viewProject;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);



VSOutPut main(VSInput input)
{
    VSOutPut output;
    output.pos = mul(input.pos, gViewProjection.viewProject);
    output.color = input.color;
    return output;
}
