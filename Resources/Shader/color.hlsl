//���ο������ڷ� https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models

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

    const int waveCount = 3; // �ĵ��� ����
    float amplitudes[waveCount] = { 10.0f, 8.0f, 9.0f }; // �� �ĵ��� ����
    float wavelengths[waveCount] = { 200.0f, 150.0f, 300.0f }; // �� �ĵ��� ����
    float speeds[waveCount] = { 3.0f, 2.5f, 4.0f }; // �� �ĵ��� �ӵ�
    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.0f)), 
        normalize(float2(0.5f, 0.5f)), 
        normalize(float2(-0.7f, 0.3f)) 
    };

    // �ʱ� ��ġ
    float3 modifiedPos = pos;

    // �� �ĵ��� ���� ��ġ ���� ���
    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i]; // ���ļ� ���
        float phase = speeds[i] * Time; // �ð��� ���� ���� ��ȭ
        float wave = sin(dot(waveDirections[i], pos.xz) * frequency + phase); // �ĵ� ���
        modifiedPos.y += amplitudes[i] * wave; // ���� ���� �߰�
    }

    return modifiedPos;
}



VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float3 modifiedPos = WaveGeneration(input.pos);
 
    // ����, ��, �������� ��ȯ
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