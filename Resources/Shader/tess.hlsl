#include "light.hlsl"

#define MAX_LIGHTS 5 


cbuffer lighting : register(b0)
{
    float3 g_eyeWorld;
    int g_lightCount;
    Light g_lights[MAX_LIGHTS];
};

cbuffer TEST_B0 : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b2)
{
    row_major matrix WorldMat;
};

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : POSITION;

};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = float4(input.pos, 1.0f);
    
    return output;
}


//«ÊΩ¶¿Ã¥ı 

struct HS_OUT
{
    float3 pos : POSITION;
};


struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

PatchConstOutput MyPatchConstantFunc(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    float3 worldPos[4];
    
    for (int i = 0; i < 4; ++i)
    {
        worldPos[i] = mul(patch[i].pos, WorldMat).xyz;
    }
    
    float3 center = (worldPos[0] + worldPos[1] + worldPos[2] + worldPos[3]) * 0.25f;
    float dist = length(center - g_eyeWorld);
    
    float distMin = 1.0f;
    float distMax = 1000.0;
   
    float tess = 64.0 * saturate((distMax - dist) / (distMax - distMin)) + 1.0;
    
    PatchConstOutput pt;
    
    pt.edges[0] = tess;
    pt.edges[1] = tess;
    pt.edges[2] = tess;
    pt.edges[3] = tess;
    
    pt.inside[0] = tess;
    pt.inside[1] = tess;
	
    return pt;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("MyPatchConstantFunc")]
[maxtessfactor(64.0f)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> p,uint i : SV_OutputControlPointID,uint patchId : SV_PrimitiveID)
{
    HS_OUT hout;
	
    hout.pos = p[i].pos.xyz;

    return hout;
}

//µµ∏ﬁ¿Œ Ω¶¿Ã¥ı
struct DS_OUT
{
    float4 pos : SV_POSITION;
};

[domain("quad")]
DS_OUT DS_Main(PatchConstOutput patchConst, float2 uv : SV_DomainLocation, const OutputPatch<HS_OUT, 4> quad)
{
    DS_OUT dout;

	// Bilinear interpolation.
    float3 v1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 v2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(v1, v2, uv.y);
    
    dout.pos = float4(p, 1.0);
    
    dout.pos = mul(dout.pos, WorldMat);
    dout.pos = mul(dout.pos, ViewMat);
    dout.pos = mul(dout.pos, ProjMat);
	
    return dout;
}


float4 PS_Main(DS_OUT input) : SV_Target
{
   
    return float4(1, 0, 0, 1);

 
}