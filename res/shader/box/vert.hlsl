
#include "common.hlsl"

[[vk::push_constant]]
ConstantBuffer<PassInfo> b_push_constant : register(b0, space0);

[[vk::binding(0, 0)]]
cbuffer Object : register(b1, space0)
{
    ObjectInfo b_object;
};

VertexOutput main(VertexAttribute attribute, out float4 position : SV_Position)
{
    VertexOutput output;
    float4 pos = mul(b_push_constant.view, mul(b_object.model, float4(attribute.position, 1.0)));
    output.texcoord = attribute.texcoord;
    output.normal = mul(b_push_constant.view, mul(float4(attribute.normal, 1.0), b_object.model_inv)).xyz;

    position = mul(b_push_constant.proj, pos);
    output.position = pos.xyz;

    return output;
}