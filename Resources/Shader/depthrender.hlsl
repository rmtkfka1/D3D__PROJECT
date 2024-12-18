

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
SamplerState g_sam_1 : register(s1);
struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float depth = g_tex_0.Sample(g_sam_1, input.uv).r;
    return float4(depth, depth, depth, 1.0f);
}