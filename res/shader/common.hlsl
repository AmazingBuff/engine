struct VertexAttribute
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// view space, because light position is defined in world space,
// so to avoid 
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
    // must transform to view space in cpu
    float3 pos;
    float3 color;
};