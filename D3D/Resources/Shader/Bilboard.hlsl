#include "light.hlsl"

#define MAX_LIGHTS 5 


cbuffer lighting : register(b0)
{
    float3 g_eyeWorld;
    int g_lightCount;
    Light g_lights[MAX_LIGHTS];
};


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
};

struct VS_OUT
{
    float4 pos : SV_Position;

};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.0f);
    
    return output;
}


struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)] // 최대 출력 Vertex 갯수
void GS_Main(point VS_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_OUT> outputStream)
{
    
    GS_OUT output;
    
    output.pos = input[0].pos + float4(0, 0, 0, 0); //좌하단
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = float2(0, 1.0f);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(0, 0.1f, 0, 0); //좌상단
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = float2(0, 0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(0.1f, 0, 0, 0); //우하단
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = float2(1.0f, 1.0f);
    output.primID = primID;
    outputStream.Append(output);

    output.pos = input[0].pos + float4(0.1f, 0.1f, 0, 0); //우상단
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = float2(1.0f, 0.0f);
    output.primID = primID;
    outputStream.Append(output);
 
   
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, input.uv);
}