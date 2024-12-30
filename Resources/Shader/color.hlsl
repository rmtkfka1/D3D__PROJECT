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

float3 WaveGenaration(float3 pos)
{
       // �ĵ� �Ķ����
    float amplitude = 100.0f; // ���� (�ĵ��� ����)
    float wavelength = 200.0f; // ���� (���� �� �Ÿ�)
    float speed = 3.0f; // �ĵ��� �ӵ�

    // ���ļ��� �ĵ� ���� ���
    float frequency = 2 * PI / wavelength;
    float2 waveDirection = normalize(float2(1.0f, 0.0f)); // �ĵ��� ���� (X, Z ��鿡��)
    
    // �ð��� ���� ���� ��ȭ
    float phase = speed * Time;

    // ���� ���� ��� (�ĵ��� ���� ����)
    float3 displacement = amplitude * sin(frequency * (pos.x * waveDirection.x + pos.z * waveDirection.y) + phase);

    // ������ ���� ���⿡ ���� ���� �� ���� ���� ���
    float3 modifiedPos = pos;
    modifiedPos.y += displacement; // ���� ����

 

    return modifiedPos;

}



VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float3 modifiedPos =WaveGenaration(input.pos);
 
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