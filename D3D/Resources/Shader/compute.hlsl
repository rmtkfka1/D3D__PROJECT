
RWTexture2D<float4> result : register(u0);
Texture2D g_tex_0 : register(t0);


[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    
    float4 texValue = g_tex_0[threadIndex.xy];

    // result에 수정된 값 기록
    result[threadIndex.xy] = texValue;
}

