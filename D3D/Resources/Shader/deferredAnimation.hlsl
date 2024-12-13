
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
    
    int enemyDraw;
    int intparams2;
    int intparams3;
    int reflectOn;
    
    float floatparams1;
    float floatparams2;
    float floatparams3;
    float floatparams4;
    
    int diffuseOn;
    int NormalOn;
    int specon;
    int texon4;

    row_major float4x4 shadowCameraVP;
    row_major float4x4 reflectMat;
 

};

cbuffer BoneMatrix : register(b4)
{
    row_major matrix BoneMatrix[256];
};



Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D shadowTexture : register(t3);
SamplerState g_sam_0 : register(s0);
SamplerState PointSampling : register(s1);
struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float4 indices : BlendIndices;
    float4 weights : BlendWeights;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float3 worldBinormal : BINORMAL;
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
    VS_OUT output;
    
    Matrix aniMat = CalculateAnimation(input);

    float4 worldPos = mul(float4(input.pos, 1.0f), aniMat);
    worldPos = mul(worldPos, WorldMat);
    
    if (reflectOn==1)
    {
        worldPos = mul(worldPos, reflectMat);
    }
    
    output.worldPos = worldPos.xyz;

    float4 viewPos = mul(worldPos, ViewMat);
    
    output.pos = mul(viewPos, ProjMat);

    output.uv = input.uv;

    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMat).xyz);
    
    if (NormalOn)
    {
       
        output.worldTangent = normalize(mul(float4(input.tangent, 0.0f), WorldMat).xyz);
        output.worldBinormal = normalize(cross(output.worldNormal, output.worldTangent));
    }
    
  
    return output;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};



PS_OUT PS_Main(VS_OUT input) : SV_Target
{
   
    PS_OUT output;
   
    output.position = float4(input.worldPos, 1.0f);
    
    output.color = diffuseTexture.Sample(g_sam_0, input.uv);
    

    // 기본 색상 텍스처 샘플링
    
    
    if (enemyDraw)
    {
        output.color += float4(0.2f, 0, 0, 0);
    }
    
    output.normal = float4(input.worldNormal, 0.0f);
 
   
    
    if (NormalOn)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = normalTexture.Sample(PointSampling, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.worldTangent, input.worldBinormal, input.worldNormal };
        output.normal = float4(mul(tangentSpaceNormal, matTBN), 0.0f);
    }
   
    
    
    return output;

};