[[vk::binding(0, 0)]]
Texture2D<float4> t_src_texture : register(t0, space0);
[[vk::binding(0, 1)]]
RWTexture2D<float4> u_dst_texture : register(u0, space1);
[[vk::binding(0, 2)]]
SamplerState s_sampler : register(s0, space2);

// the dimension of src texture must be equal to dst texture

[numthreads(8, 8, 1)]
void edge_detect(uint3 thread_id : SV_DispatchThreadID)
{
	const float threshold = 0.5f;
	const float3 coefficient = float3(0.2989, 0.5870, 0.1141);

    uint dst_width, dst_height;
    u_dst_texture.GetDimensions(dst_width, dst_height);

    float2 uv = (float2(thread_id.xy) + 0.5) / float2(dst_width, dst_height);
    float4 src_color = t_src_texture.Sample(s_sampler, uv);
	float4 left = t_src_texture.Sample(s_sampler, uv + float2(-1 / dst_width, 0));
	float4 top = t_src_texture.Sample(s_sampler, uv + float2(0, 1 / dst_height));

	float i_c = dot(src_color.rgb, coefficient);
	float i_l = dot(left.rgb, coefficient);
	float i_t = dot(top.rgb, coefficient);

	float2 delta = abs(i_c - float2(i_l, i_t));
	float2 edge = step(threshold, delta);

	t_dst_texture[thread_id.xy] = float4(edge, 0, 0);
}