RWTexture2D<float4> resultTexture : register(u0); // ��� �ؽ�ó
Texture2D<float4> inputTexture : register(t0); // �Է� �ؽ�ó

SamplerState samplerState;

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

// ������ �׷� ũ�� ����
[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    // ���� �ȼ� ��ǥ
    int2 texCoord = threadIndex.xy;

    // �ؽ�ó ũ�� �ʰ� ��ǥ ó��
    if (texCoord.x >= window_width || texCoord.y >= window_height)
        return;

    float4 sum = float4(0, 0, 0, 0); // ���� �ջ�
    int sampleCount = 0;

    // �� ���� �ݰ�
    int radius = 2;

    // �ڽ� �� ����
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            int2 sampleCoord = texCoord + int2(x, y);

            // �ؽ�ó ��� ���� Ȯ��
            if (sampleCoord.x >= 0 && sampleCoord.x < window_width &&
                sampleCoord.y >= 0 && sampleCoord.y < window_height)
            {
                // ���ø� ���� (��Ȯ�� ��ǥ���� �ؽ�ó �б�)
                sum += inputTexture.Load(int3(sampleCoord, 0));
                sampleCount++;
            }
        }
    }

    // ��հ� ���
    float4 blurredColor = sum / sampleCount;

    // ��� �ؽ�ó�� ����
    resultTexture[texCoord] = blurredColor;
}