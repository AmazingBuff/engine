
#include "common.hlsl"

[[vk::push_constant]]
ConstantBuffer<PassInfo> b_push_constant : register(b0, space0);

[[vk::binding(1, 0)]]
ConstantBuffer<ObjectInfo> b_object : register(b1, space0);

[[vk::binding(0, 1)]]
Texture2D<float4> t_texture : register(t0, space1);

[[vk::binding(0, 2)]]
SamplerState s_sampler : register(s0, space2);

[[vk::binding(2, 0)]]
ConstantBuffer<LightInfo> b_light : register(b2, space0);

VertexOutput vs(VertexAttribute attribute, out float4 position : SV_Position)
{
    VertexOutput output;
    float4 pos = mul(b_push_constant.view, mul(b_object.model, float4(attribute.position, 1.0)));
    output.texcoord = attribute.texcoord;
    output.normal = mul(b_push_constant.view, mul(float4(attribute.normal, 1.0), b_object.model_inv)).xyz;

    position = mul(b_push_constant.proj, pos);
    output.position = pos.xyz;

    return output;
}

float4 ps(VertexOutput input) : SV_Target
{
    float4 frag_color;
    
    float4 diffuse = t_texture.Sample(s_sampler, input.texcoord);
    
    float3 l = input.position - b_light.pos;
    float3 v = 0.0;
    
    
    return diffuse;
}