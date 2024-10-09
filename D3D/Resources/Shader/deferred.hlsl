
cbuffer TEST_B0 : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b2)
{
    row_major matrix WorldMat;
};

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);
    output.worldPos = worldPos.xyz;

    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);

    output.uv = input.uv;
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMat).xyz);
    

    return output;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};


PS_OUT PS_Main(VS_OUT input) : SV_Target
{
   
    PS_OUT output;

    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(input.worldNormal, 0.0f);
    output.color = g_tex_0.Sample(g_sam_0, input.uv);

    return output;

}