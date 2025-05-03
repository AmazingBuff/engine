struct VertexAttribute
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// view space
struct VertexOutput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
};

struct ObjectInfo
{
    float4x4 model;
    float4x4 model_inv;
};

struct PassInfo
{
    float4x4 view;
    float4x4 proj;
};

struct LightInfo
{
    float3 pos;
    float3 color;
};