#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

RWTexture2D<float4> g_tex : register(u0);


[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.y % 2 == 0)
        g_tex[threadIndex.xy] = float4(1.f, 0.f, 0.f, 1.f);
    else
        g_tex[threadIndex.xy] = float4(0.f, 1.f, 0.f, 1.f);
}

#endif