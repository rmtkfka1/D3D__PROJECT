RWTexture2D<float4> resultTexture : register(u0);
Texture2D<float4> inputTexture : register(t0);

// 샘플러 선언
SamplerState samplerState;

// 블러 필터 반경 및 밝기 임계값 설정
static const int blurRadius = 5;
static const float threshold = 0.8;
static const int numBlurPasses = 50; // 블러 반복 횟수 설정 (다단계 블러)

// 스레드 그룹 크기 정의
[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    int2 texCoord = threadIndex.xy;

    // 텍스처 크기 가져오기
    uint2 textureSize = uint2(1024, 800);

    // 1. 초기 색상 로드 (원본)
    float4 originalColor = inputTexture.Load(int3(texCoord, 0));

    // 2. 여러 번의 가우시안 블러 적용 (다단계)
    float4 blurredColor = originalColor;
    float totalWeight = 0.0;

    for (int i = 0; i < numBlurPasses; i++)
    {
        // 수평 방향 블러
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

        // 수평 블러 정규화
        tempColor /= totalWeight;

        // 수직 방향 블러
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

        // 수직 블러 정규화
        blurredColor = tempColor / totalWeight;
    }

    // 3. 블룸 효과 적용
    // 밝기 계산 (Luminance)
    float brightness = dot(originalColor.rgb, float3(0.2126, 0.7152, 0.0722));

    // 밝기 임계값 적용
    float4 bloomEffect = brightness > threshold ? blurredColor : originalColor;

    // 4. 최종 결과 병합 (원본 + 블룸 효과)
    resultTexture[texCoord] = originalColor + bloomEffect * 0.8; // 블룸 강도 0.8
}
