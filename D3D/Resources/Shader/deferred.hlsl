
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
    int intparams4;
    
    float floatparams1;
    float floatparams2;
    float floatparams3;
    float floatparams4;
    
    int diffuseOn;
    int NormalOn;
    int specon;
    int texon4;

    
 

};

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;

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


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);
    output.worldPos = worldPos.xyz;

    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);

    output.uv = input.uv;

    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), WorldMat).xyz);
    
    if (NormalOn)
    {
       
        output.worldTangent = normalize(mul(float4(input.tangent, 0.0f), WorldMat).xyz);
        output.worldBinormal = normalize(cross(output.worldNormal, output.worldTangent));
        //output.worldBinormal = normalize(cross(output.worldTangent, output.worldNormal));
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
    

    // �⺻ ���� �ؽ�ó ���ø�
    
    
    if (enemyDraw)
    {
        output.color += float4(0.2f, 0, 0, 0);
    }
    
    output.normal = float4(input.worldNormal, 0.0f);
 
   
    
    if (NormalOn )
    {
        // [0,255] �������� [0,1]�� ��ȯ
        float3 tangentSpaceNormal = normalTexture.Sample(g_sam_0, input.uv).xyz;
        // [0,1] �������� [-1,1]�� ��ȯ
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.worldTangent, input.worldBinormal, input.worldNormal };
        output.normal = float4(mul(tangentSpaceNormal, matTBN), 0.0f);
    }


    return output;

}