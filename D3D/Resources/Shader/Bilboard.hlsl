#include "Light.hlsl"

#define MAX_LIGHTS 5 



cbuffer TEST_B0 : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b2)
{
    row_major matrix WorldMat;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 uv: TEXCOORD;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};



VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = float4(input.pos, 1.f);
   
    return output;
}

[maxvertexcount(4)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
   
    float hw = 5.0f;
    GS_OUT output;
    

    {
        output.pos = input[0].pos;
        output.pos.x -= hw;
        output.pos.y += hw;
        
        float4 WorldPos = mul(output.pos, WorldMat);
        output.worldPos = WorldPos.xyz;
        
        output.uv = float2(0, 0);
        output.normal = float3(0, 0, 1.0f);
        output.normal = mul(float4(output.normal, 0.0f), WorldMat);
        output.normal = normalize(output.normal);
        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);

        outputStream.Append(output);
    }
    

    {
        output.pos = input[0].pos;
        output.pos.x += hw;
        output.pos.y += hw;
        
        float4 WorldPos = mul(output.pos, WorldMat);
        output.worldPos = WorldPos.xyz;
        
        output.uv = float2(1, 0);
        output.normal = float3(0, 0, 1.0f);
        output.normal = mul(float4(output.normal, 0.0f), WorldMat);
        output.normal = normalize(output.normal);

        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);

        outputStream.Append(output);
    }
    

    {
        output.pos = input[0].pos;
        output.pos.x -= hw;
        output.pos.y -= hw;
        
        float4 WorldPos = mul(output.pos, WorldMat);
        output.worldPos = WorldPos.xyz;
        
        output.uv = float2(0, 1);
        output.normal = float3(0, 0, 1.0f);
        output.normal = mul(float4(output.normal, 0.0f), WorldMat);
        output.normal = normalize(output.normal);

        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);

        outputStream.Append(output);
        }
    

     {
        output.pos = input[0].pos;
        output.pos.x += hw;
        output.pos.y -= hw;
        
        float4 WorldPos = mul(output.pos, WorldMat);
        output.worldPos = WorldPos.xyz;
        
        output.uv = float2(1, 1);
        output.normal = float3(0, 0, 1.0f);
        output.normal = mul(float4(output.normal, 0.0f), WorldMat);
        output.normal = normalize(output.normal);

        output.pos = mul(output.pos, WorldMat);
        output.pos = mul(output.pos, ViewMat);
        output.pos = mul(output.pos, ProjMat);
        outputStream.Append(output);
    }
    
   
   
}


PS_OUT PS_Main(GS_OUT input)
{
  
    PS_OUT output;

    output.position = float4(input.worldPos, 1.0f);
    output.color = diffuseTexture.Sample(g_sam_0, input.uv);
    output.normal = float4(normalize(input.normal), 0.0f);
 
    float sumColor = (output.color.x + output.color.y + output.color.z) / 3.0f;
    
    clip(sumColor > 0.8f ? -1 : 1);
    
    clip(output.color.a - 0.9f);
    
    return output;

}