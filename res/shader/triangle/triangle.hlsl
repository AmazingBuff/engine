static const float2 positions[3] =
{
    float2(0.0, 0.5),
    float2(-0.5, -0.5),
    float2(0.5, -0.5)
};

static const float3 colors[3] =
{
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

float3 vs(uint VertexIndex : SV_VertexID, out float4 position : SV_POSITION) : COLOR
{
    float3 color = colors[VertexIndex];

    position = float4(positions[VertexIndex], 0.f, 1.f);

    return color;
}

float4 ps(float3 color : COLOR) : SV_TARGET
{
    return float4(color, 1.f);
}