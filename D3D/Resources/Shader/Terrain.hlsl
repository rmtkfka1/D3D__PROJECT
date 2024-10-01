
cbuffer TEST_B0 : register(b0)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b1)
{
    row_major matrix WorldMat;
};

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);

SamplerState g_sam_0 : register(s0);

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

    output.pos = mul(float4(input.pos, 1.f), WorldMat);
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = input.uv;
  
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   
    float color = g_tex_0.Sample(g_sam_0, input.uv);
    color += g_tex_1.Sample(g_sam_0, input.uv *2.0f);
    color += g_tex_2.Sample(g_sam_0, input.uv * 0.3f);
    return color;

 
}