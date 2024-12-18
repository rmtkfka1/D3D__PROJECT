#include "light.hlsl"

#define MAX_LIGHTS 5 


cbuffer lighting : register(b0)
{
    float3 g_eyeWorld;
    int g_lightCount;
    Light g_lights[MAX_LIGHTS];
};


cbuffer VPMatrix : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer WorldMatrix : register(b2)
{
    row_major matrix WorldMat;
};


cbuffer BoneMatrix : register(b4)
{
    row_major matrix BoneMatrix[256];
};



Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 indices : BlendIndices;
    float4 weights : BlendWeights;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};


Matrix CalculateAnimation(VS_IN input)
{
   // 행렬 초기화
    float4x4 mat = 0;

    // Bone indices and weights
    uint index1 = (uint) input.indices.x;
    uint index2 = (uint) input.indices.y;
    uint index3 = (uint) input.indices.z;
    uint index4 = (uint) input.indices.w;

    float weight1 = input.weights.x;
    float weight2 = input.weights.y;
    float weight3 = input.weights.z;
    float weight4 = input.weights.w;

  
    mat += weight1 * BoneMatrix[index1];
    mat += weight2 * BoneMatrix[index2];
    mat += weight3 * BoneMatrix[index3];
    mat += weight4 * BoneMatrix[index4];

    return mat;
    
    
    
    
}


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    Matrix aniMat = CalculateAnimation(input);
    float4 worldPos = mul(float4(input.pos, 1.0f), aniMat);
    worldPos = mul(worldPos, WorldMat);
    output.worldPos = worldPos.xyz;

    // 뷰 및 투영 변환 적용
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);

    // 노멀 변환 (평행 이동 제외)
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMat));
    
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
    
    return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);

 
}