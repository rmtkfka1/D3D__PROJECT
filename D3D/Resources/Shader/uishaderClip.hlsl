

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
    
    float iTime;
    float dx;
    float dy;
    float strength;

    int texon1;
    int texon2;
    int texon3;
    int texon4;
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

    // ���� ��ǥ ���
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);

    // �� �� ���� ��ȯ ����
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