
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
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D shadowTexture : register(t3);
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);

cbuffer materialparams : register(b3)
{
    
    int enemyDraw;
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
    VS_OUT output = (VS_OUT) 0;

    // 월드 좌표 계산
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);
    output.worldPos = worldPos.xyz;

    // 뷰 및 투영 변환 적용
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);

    // 노멀 변환 후 정규화 (평행 이동 제외)
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMat).xyz);
    
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   
    float3 color = float3(0, 0, 0);
    
    float3 toEye = normalize(g_eyeWorld - input.worldPos);
    
    for (int i = 0; i < g_lightCount; ++i)
    {
   
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, input.worldNormal, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, input.worldPos, input.worldNormal, toEye);
            
       
        }
        else if (g_lights[i].mateiral.lightType == 2)
        {
            color += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, input.worldPos, input.worldNormal, toEye);
        }
          
    }
    
    float4 result = float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
    
    result += g_tex_1.Sample(g_sam_0, input.uv * 2.0f);

    result += g_tex_2.Sample(g_sam_0, input.uv * 0.20f);
    
    result = clamp(result, 0, 1.0f);
    
    matrix shadowCameraVP = g_mat_0;
    float4 shadowClipPos = mul(float4(input.worldPos, 1.0f), shadowCameraVP);
    float depth = shadowClipPos.z / shadowClipPos.w;
    float2 uv = shadowClipPos.xy / shadowClipPos.w;
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
    
    float shadowDepth = shadowTexture.Sample(g_sam_1, uv).r;
    
    if (uv.x >= 0 && uv.x <= 1.0f && uv.y >= 0 && uv.y <= 1.0f)
    { 
        if (shadowDepth > 0 && depth > shadowDepth + 0.003f)
        {
            result *= 0.3f;
        }
    }
    
    return result;
}