

cbuffer TEST_B0 : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b2)
{
    row_major matrix WorldMat;
};


cbuffer materialparams : register(b3)
{
   
    int intparams1;
    int intparams2;
    int intparams3;
    int intparams4;
    
    float floatparams1;
    float floatparams2;
    float floatparams3;
    float floatparams4;
    
    int diffuseOn;
    int NormalOn;
    int specon;
    int texon4;

    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
 
};
Texture2D g_tex_0 : register(t0);
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

    // 월드 좌표 계산
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);

    // 뷰 및 투영 변환 적용
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    if (intparams1==0)
    {
    
        float4 color = g_tex_0.Sample(g_sam_0, input.uv);
        float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));
        clip(luminance - 0.2);
        return color;
    }
    
    else
    {

        float4 color = g_tex_0.Sample(g_sam_0, input.uv);
        float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));
        clip(luminance - 0.9f);
        return color * float4(1, 0, 0, 0);
    }
}