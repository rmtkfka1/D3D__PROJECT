#include "Light.hlsl"

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
    float4 pos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 uv: TEXCOORD;
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


float4 PS_Main(GS_OUT input) : SV_Target
{
  
    float4 texColor = g_tex_0.Sample(g_sam_0, input.uv);
    
    float sumColor = (texColor.x + texColor.y + texColor.z) / 3.0f;
    
    clip(sumColor > 0.8f ? -1 : 1);
    
    clip(texColor.a - 0.9f);
    
    
    float3 color = float3(0, 0, 0);
    
    float3 toEye = normalize(g_eyeWorld - input.worldPos);
    
    for (int i = 0; i < g_lightCount; ++i)
    {
   
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, input.normal, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, input.worldPos, input.normal, toEye);
            
       
        }
        else if (g_lights[i].mateiral.lightType == 2)
        {
            color += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, input.worldPos, input.normal, toEye);
        }
          
    }
    
    return float4(color, 1.0f) * texColor;

}