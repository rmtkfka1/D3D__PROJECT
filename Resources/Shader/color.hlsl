//공부용참고자료 https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models

#define PI 3.14159f

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
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

float3 WaveGeneration(float3 pos)
{

    const int waveCount = 3; // 파동의 개수
    float amplitudes[waveCount] = { 10.0f, 8.0f, 9.0f }; // 각 파동의 진폭
    float wavelengths[waveCount] = { 200.0f, 150.0f, 300.0f }; // 각 파동의 파장
    float speeds[waveCount] = { 3.0f, 2.5f, 4.0f }; // 각 파동의 속도
    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.0f)), 
        normalize(float2(0.5f, 0.5f)), 
        normalize(float2(-0.7f, 0.3f)) 
    };

    // 초기 위치
    float3 modifiedPos = pos;

    // 각 파동에 대해 위치 변위 계산
    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i]; // 주파수 계산
        float phase = speeds[i] * Time; // 시간에 따른 위상 변화
        float wave = sin(dot(waveDirections[i], pos.xz) * frequency + phase); // 파동 계산
        modifiedPos.y += amplitudes[i] * wave; // 수직 변위 추가
    }

    return modifiedPos;
}



VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float3 modifiedPos = WaveGeneration(input.pos);
 
    // 월드, 뷰, 프로젝션 변환
    float4 worldPos = mul(float4(modifiedPos, 1.0f), WorldMat);
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);

    output.uv = input.uv;
    return output;
}
float4 PS_Main(VS_OUT input) : SV_Target
{
   float4 color = float4(1, 0, 0, 0);
    
    return color;
}