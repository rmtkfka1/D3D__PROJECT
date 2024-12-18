struct VS_IN
{
    float3 localPos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD;
};

cbuffer CameraParams : register(b1)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
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

    row_major float4x4 reflectMat;
    row_major float4x4 g_mat_1;
 
};


TextureCube g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // ���� ��ǥ�� �״�� ����
    output.localPos.x = input.localPos.y;

    // Translation�� ���� �ʰ� Rotation�� �����Ѵ�
    float4 viewPos = mul(float4(input.localPos, 0), ViewMatrix);
    float4 clipSpacePos = mul(viewPos, ProjectionMatrix);

    // w/w=1�̱� ������ �׻� ���̰� 1�� �����ȴ�
    output.pos = clipSpacePos.xyww;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // ť��� �ؽ�ó�� ���� ��ǥ�� ����Ͽ� ���ø�
    float4 color = g_tex_0.Sample(g_sam_0, input.localPos);
    return color;
}