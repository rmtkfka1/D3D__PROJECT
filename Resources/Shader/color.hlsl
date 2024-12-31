//���ο������ڷ� https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models


#define PI 3.14159f
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

cbuffer materialparams : register(b3)
{
    
    int intparams1;
    int intparams2;
    int intparams3;
    int intparams4;
    
    float Time;
    float floatparams2;
    float floatparams3;
    float floatparams4;
    
    int diffuseOn;
    int NormalOn;
    int specon;
    int texon4;

    row_major float4x4 shadowCameraVP;
    row_major float4x4 g_mat_1;
 
};



Texture2D g_tex_0 : register(t0);
TextureCube g_tex_1 : register(t1);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 worldNormal : NORMAL;
};

VS_IN WaveGeneration(VS_IN input)
{
    const int waveCount = 3; // �ĵ��� ����
    float amplitudes[waveCount] = { 12.0f, 6.0f, 4.0f }; // �� �ĵ��� ���� (���̸� �ٿ� �ڿ�������)
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f }; // �� �ĵ��� ���� (�� ���� ����)
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f }; // �� �ĵ��� �ӵ� (�ӵ� ����)

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)), // �� ���� (���� �ϴ� �������� ����)
        normalize(float2(0.0f, 1.0f)), // ���� ���� (���� ����)
        normalize(float2(-0.5f, 0.7f)) // �밢�� ���� (���� ���� ����)
    };

    // �ʱ� ��ġ
    float3 modifiedPos = input.pos;
    float3 modifiedNormal = float3(0.0f, 0.0f, 0.0f); // �ʱ�ȭ ����

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

    // xz �� y ���� ���� ����
        modifiedPos.xz += amplitudes[i] * direction * waveDerivative;
        modifiedPos.y += amplitudes[i] * wave;

    // ���� ���� ���
        float3 tangentX = float3(direction.x, waveDerivative * direction.x, 0.0f);
        float3 tangentZ = float3(0.0f, waveDerivative * direction.y, direction.y);

    // ���� �⿩�� �ջ�
        modifiedNormal += cross(tangentX, tangentZ);
    }

    modifiedNormal = normalize(modifiedNormal); // ����ȭ

    VS_IN result;
    result.pos = modifiedPos;
    result.normal = modifiedNormal;

    return result;
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    VS_IN result = WaveGeneration(input);
 
    // ����, ��, �������� ��ȯ
    float4 worldPos = mul(float4(result.pos, 1.0f), WorldMat);
    
    output.worldPos = worldPos.xyz;
    
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);
    output.uv = input.uv;
    output.worldNormal = normalize(mul(float4(result.normal, 0.0f), WorldMat).xyz);
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    
    float3 toEye = normalize(g_eyeWorld - input.worldPos.xyz);
    
    float3 color = float3(0, 0, 0);
    
    for (int i = 0; i < g_lightCount; ++i)
    {
   
        if (g_lights[i].mateiral.lightType == 0)
        {
        
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, input.worldNormal.xyz, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, input.worldPos.xyz, input.worldNormal.xyz, toEye);
        }
      
    }
    
    
    return float4(color, 1.0f);
}
