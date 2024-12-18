RWTexture2D<float4> resultTexture : register(u0); 
Texture2D<float4> blurrTexture : register(t0); 
Texture2D<float4> originTexture : register(t1); 
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


static float weight = 0.8f;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    if (texCoord.x<0 || texCoord.x >= window_width || texCoord.y <0 || texCoord.y >= window_height)
        return;

    
    float3 color;
 
    color = clamp(blurrTexture[texCoord] * weight + originTexture[texCoord], 0, 1.0f);
  
    resultTexture[texCoord] = float4(color, 1.0f);
}