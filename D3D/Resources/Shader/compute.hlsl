RWTexture2D<float4> resultTexture : register(u0); // 결과 텍스처
Texture2D<float4> inputTexture : register(t0); // 입력 텍스처

SamplerState samplerState;

cbuffer materialparams : register(b3)
{
    int window_width; // 윈도우 너비
    int window_height; // 윈도우 높이
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

// 스레드 그룹 크기 정의
[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    // 현재 픽셀 좌표
    int2 texCoord = threadIndex.xy;

    // 텍스처 크기 초과 좌표 처리
    if (texCoord.x >= window_width || texCoord.y >= window_height)
        return;

    float4 sum = float4(0, 0, 0, 0); // 색상 합산
    int sampleCount = 0;

    // 블러 필터 반경
    int radius = 2;

    // 박스 블러 필터
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            int2 sampleCoord = texCoord + int2(x, y);

            // 텍스처 경계 조건 확인
            if (sampleCoord.x >= 0 && sampleCoord.x < window_width &&
                sampleCoord.y >= 0 && sampleCoord.y < window_height)
            {
                // 샘플링 수행 (정확한 좌표에서 텍스처 읽기)
                sum += inputTexture.Load(int3(sampleCoord, 0));
                sampleCount++;
            }
        }
    }

    // 평균값 계산
    float4 blurredColor = sum / sampleCount;

    // 결과 텍스처에 쓰기
    resultTexture[texCoord] = blurredColor;
}