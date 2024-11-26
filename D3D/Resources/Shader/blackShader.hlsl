RWTexture2D<float4> resultTexture : register(u0); // ��� �ؽ�ó
Texture2D<float4> inputTexture : register(t0); // �Է� �ؽ�ó

cbuffer materialparams : register(b3)
{
    int window_width; // ������ �ʺ�
    int window_height; // ������ ����
    int on;
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

static float Luminance[3] = { 0.2126f, 0.7152f, 0.072f };
static float th = 0.7f;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    if (texCoord.x<0 || texCoord.x >= window_width || texCoord.y <0 || texCoord.y >= window_height)
        return;
    
    float3 color = inputTexture[texCoord];
    
    float relativeLuminance = (color.r * Luminance[0]) + (color.g * Luminance[1]) + (color.b * Luminance[2]);
    
    if (relativeLuminance<th)
    {
        color = float3(0, 0, 0);
    }
    
   
    // ��� �ؽ�ó�� ����
    resultTexture[texCoord] = float4(color, 1.0f);
}