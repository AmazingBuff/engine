//[[vk::binding(0, 0)]]
Texture2D<float4> sampled_texture : register(t0, space0);

//[[vk::binding(1, 0)]]
SamplerState texture_sampler : register(s0, space1);

float4 main(float2 uv : TEXCOORD0) : SV_TARGET
{
    return sampled_texture.Sample(texture_sampler, uv);
}