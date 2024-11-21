RWTexture2D<float4> resultTexture : register(u0);
Texture2D<float4> inputTexture : register(t0);

// ���÷� ����
SamplerState samplerState;

// �� ���� �ݰ� �� ��� �Ӱ谪 ����
static const int blurRadius = 5;
static const float threshold = 0.8;
static const int numBlurPasses = 50; // �� �ݺ� Ƚ�� ���� (�ٴܰ� ��)

// ������ �׷� ũ�� ����
[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    int2 texCoord = threadIndex.xy;

    // �ؽ�ó ũ�� ��������
    uint2 textureSize = uint2(1024, 800);

    // 1. �ʱ� ���� �ε� (����)
    float4 originalColor = inputTexture.Load(int3(texCoord, 0));

    // 2. ���� ���� ����þ� �� ���� (�ٴܰ�)
    float4 blurredColor = originalColor;
    float totalWeight = 0.0;

    for (int i = 0; i < numBlurPasses; i++)
    {
        // ���� ���� ��
        float4 tempColor = float4(0, 0, 0, 0);
        totalWeight = 0.0;

        for (int x = -blurRadius; x <= blurRadius; x++)
        {
            int2 offset = int2(x, 0);
            int2 sampleCoord = texCoord + offset;

            if (sampleCoord.x >= 0 && sampleCoord.x < textureSize.x &&
                sampleCoord.y >= 0 && sampleCoord.y < textureSize.y)
            {
                float weight = exp(-0.5 * (x * x) / (blurRadius * blurRadius));
                tempColor += inputTexture.SampleLevel(samplerState, sampleCoord / (float2) textureSize, 0) * weight;
                totalWeight += weight;
            }
        }

        // ���� �� ����ȭ
        tempColor /= totalWeight;

        // ���� ���� ��
        blurredColor = tempColor;
        totalWeight = 0.0;
        
        tempColor = float4(0, 0, 0, 0);

        for (int y = -blurRadius; y <= blurRadius; y++)
        {
            int2 offset = int2(0, y);
            int2 sampleCoord = texCoord + offset;

            if (sampleCoord.x >= 0 && sampleCoord.x < textureSize.x &&
                sampleCoord.y >= 0 && sampleCoord.y < textureSize.y)
            {
                float weight = exp(-0.5 * (y * y) / (blurRadius * blurRadius));
                tempColor += inputTexture.SampleLevel(samplerState, sampleCoord / (float2) textureSize, 0) * weight;
                totalWeight += weight;
            }
        }

        // ���� �� ����ȭ
        blurredColor = tempColor / totalWeight;
    }

    // 3. ��� ȿ�� ����
    // ��� ��� (Luminance)
    float brightness = dot(originalColor.rgb, float3(0.2126, 0.7152, 0.0722));

    // ��� �Ӱ谪 ����
    float4 bloomEffect = brightness > threshold ? blurredColor : originalColor;

    // 4. ���� ��� ���� (���� + ��� ȿ��)
    resultTexture[texCoord] = originalColor + bloomEffect * 0.8; // ��� ���� 0.8
}
