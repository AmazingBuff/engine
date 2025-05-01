
float2 main(uint VertexIndex : SV_VertexID, out float4 position : SV_POSITION) : TEXCOORD0
{
    float2 ret = float2((VertexIndex << 1) & 2, VertexIndex & 2);
    position = float4(ret * 2.0 - 1.0, 0.0, 1.0);

    return ret;
}