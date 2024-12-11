#include "light.hlsl"

#define MAX_LIGHTS 5 
#define MAX_BONE 250
#define MAX_FRAME 250

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


cbuffer materialparams : register(b3)
{
    int currFrame;
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
StructuredBuffer<Matrix> aniMatrix : register(t3);
SamplerState g_sam_0 : register(s0);


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHTS;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};

Matrix GetBoneTransform(VS_IN input)
{
    float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
    float weights[4] = { input.blendWeights.x, input.blendWeights.y, input.blendWeights.z, input.blendWeights.w };
    
    Matrix mat1 = weights[0] * aniMatrix[indices[0] * MAX_FRAME + indices[0]];
    Matrix mat2 = weights[1] * aniMatrix[indices[1] * MAX_FRAME + indices[1]];
    Matrix mat3 = weights[2] * aniMatrix[indices[2] * MAX_FRAME + indices[2]];
    Matrix mat4 = weights[3] * aniMatrix[indices[3] * MAX_FRAME + indices[3]];
    
    return mat1+mat2+mat3+mat4;
}


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 월드 좌표 계산
    Matrix aniMat = GetBoneTransform(input);
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