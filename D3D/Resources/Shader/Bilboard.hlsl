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

struct GS_OUT
{
    float4 pos : SV_Position;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = float4(input.pos, 1.f);
   
    return output;
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
   
    float hw = 5.0f;
    GS_OUT output;
    
    {
        output.pos = input[0].pos;
        output.pos.x -= hw;
        output.pos.y += hw;
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
    
    {
        output.pos = input[0].pos;
        output.pos.x += hw;
        output.pos.y += hw;
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
    
    {
        output.pos = input[0].pos;
        output.pos.x -= hw;
        output.pos.y -= hw;
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
    
    outputStream.RestartStrip();
    
    
     {
        output.pos = input[0].pos;
        output.pos.x += hw;
        output.pos.y += hw;
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
    
    {
        output.pos = input[0].pos;
        output.pos.x += hw;
        output.pos.y -= hw;
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
    
  
    
    {
        output.pos = input[0].pos;
        output.pos.x -= hw;
        output.pos.y -= hw;
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
   
}


float4 PS_Main(GS_OUT input) : SV_Target
{
    return float4(1, 0, 0, 1);
}