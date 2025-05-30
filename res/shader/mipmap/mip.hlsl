// only for dx12 mipmap generate
[[vk::binding(0, 0)]]
Texture2D<float4> t_src_texture : register(t0, space0);
[[vk::binding(0, 1)]]
RWTexture2D<float4> u_dst_texture : register(u0, space1);
[[vk::binding(0, 2)]]
SamplerState s_sampler : register(s0, space2);

[numthreads(8, 8, 1)]
void mipmap(uint3 thread_id : SV_DispatchThreadID)
{
    uint dst_width, dst_height;
    u_dst_texture.GetDimensions(dst_width, dst_height);
    float2 uv = (float2(thread_id.xy) + 0.5) / float2(dst_width, dst_height);
    u_dst_texture[thread_id.xy] = t_src_texture.Sample(s_sampler, uv);
}