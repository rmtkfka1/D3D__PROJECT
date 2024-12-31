//공부용참고자료 https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models


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
    const int waveCount = 3; // 파동의 개수
    float amplitudes[waveCount] = { 12.0f, 6.0f, 4.0f }; // 각 파동의 진폭 (높이를 줄여 자연스럽게)
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f }; // 각 파동의 파장 (더 넓은 범위)
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f }; // 각 파동의 속도 (속도 조정)

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)), // 주 방향 (우측 하단 방향으로 진행)
        normalize(float2(0.0f, 1.0f)), // 수직 방향 (위쪽 방향)
        normalize(float2(-0.5f, 0.7f)) // 대각선 방향 (좌측 위쪽 방향)
    };

    // 초기 위치
    float3 modifiedPos = input.pos;
    float3 modifiedNormal = float3(0.0f, 0.0f, 0.0f); // 초기화 변경

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

    // xz 및 y 방향 변위 적용
        modifiedPos.xz += amplitudes[i] * direction * waveDerivative;
        modifiedPos.y += amplitudes[i] * wave;

    // 기울기 벡터 계산
        float3 tangentX = float3(direction.x, waveDerivative * direction.x, 0.0f);
        float3 tangentZ = float3(0.0f, waveDerivative * direction.y, direction.y);

    // 법선 기여도 합산
        modifiedNormal += cross(tangentX, tangentZ);
    }

    modifiedNormal = normalize(modifiedNormal); // 정규화

    VS_IN result;
    result.pos = modifiedPos;
    result.normal = modifiedNormal;

    return result;
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    VS_IN result = WaveGeneration(input);
 
    // 월드, 뷰, 프로젝션 변환
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
