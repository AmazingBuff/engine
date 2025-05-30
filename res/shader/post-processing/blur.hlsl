// compute shader, mean blur

[[vk::binding(0, 0)]]
Texture2D<float4> t_src_texture : register(t0, space0);
[[vk::binding(0, 1)]]
RWTexture2D<float4> u_dst_texture : register(u0, space1);

struct BlurParameter
{
    int blur_radius;
    int blur_direction; // 0 for horizontal, 1 for vertical
};

[[vk::push_constant]]
ConstantBuffer<BlurParameter> b_blur_parameter : register(b0, space0);

[numthreads(8, 8, 1)]
void mean_blur(uint3 thread_id : SV_DispatchThreadID)
{
    uint width, height;
    t_src_texture.GetDimensions(width, height);
    float4 color = 0;
    for (int i = -b_blur_parameter.blur_radius; i <= b_blur_parameter.blur_radius; ++i)
    {
        int2 pos = int2(thread_id.x, thread_id.y) + lerp(int2(i, 0), int2(0, i), b_blur_parameter.blur_direction);
        pos = clamp(pos, int2(0, 0), int2(width - 1, height - 1));
        color += t_src_texture[pos];
    }
    u_dst_texture[thread_id.xy] = color / (2 * b_blur_parameter.blur_radius + 1);
}