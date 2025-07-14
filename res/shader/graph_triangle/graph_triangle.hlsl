
#include "common.hlsl"

[[vk::binding(0, 0)]]
ConstantBuffer<LightInfo> b_light : register(b0, space0);

VertexOutput vs(VertexAttribute attribute, out float4 position : SV_Position)
{
    VertexOutput output;
    output.position = attribute.position;
    output.texcoord = attribute.texcoord;
    output.normal = attribute.normal;

    position = float4(attribute.position, 1.0);

    return output;
}

float4 ps(VertexOutput input) : SV_TARGET
{
    return float4(b_light.color, 1.f);
}