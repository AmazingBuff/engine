[[vk::binding(0, 0)]]
Texture2D<float4> sampled_texture : register(t0, space0);

[[vk::binding(0, 1)]]
SamplerState texture_sampler : register(s0, space1);

float2 vs(uint VertexIndex : SV_VertexID, out float4 position : SV_POSITION) : TEXCOORD0
{
    float2 ret = float2((VertexIndex << 1) & 2, VertexIndex & 2);
    position = float4(ret * 2.0 - 1.0, 0.0, 1.0);

    return ret;
}

float4 ps(float2 uv : TEXCOORD0) : SV_TARGET
{
    return sampled_texture.Sample(texture_sampler, uv);
}