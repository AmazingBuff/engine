
#include "common.hlsl"

[[vk::binding(0, 0)]]
Texture2D<float4> t_texture : register(t0, space1);

[[vk::binding(2, 0)]]
SamplerState s_sampler : register(s0, space2);

[[vk::binding(1, 1)]]
cbuffer Light : register(b2, space0)
{
    LightInfo b_light;
};

float4 main(VertexOutput input) : SV_Target
{
    float4 frag_color;
    
    float4 diffuse = t_texture.Sample(s_sampler, input.texcoord);
    
    float3 l = input.position - b_light.pos;
    float3 v = -input.position;
    
    
    return diffuse;
}