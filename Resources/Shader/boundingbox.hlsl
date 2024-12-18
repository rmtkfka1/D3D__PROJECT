

cbuffer camera : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer world : register(b2)
{
    row_major matrix WorldMat;
};


struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = float4(input.pos, 1.f);
    
    output.pos = mul(output.pos, WorldMat);
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   
    return float4(1.0f, 0, 0, 0);
}