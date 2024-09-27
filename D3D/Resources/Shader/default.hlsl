cbuffer TEST_B0 : register(b0)
{
    row_major matrix WorldMat;
};

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), WorldMat);
    output.color = input.color ;
    output.uv = input.uv;
  
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   
   
    return g_tex_0.Sample(g_sam_0, input.uv);

 
}