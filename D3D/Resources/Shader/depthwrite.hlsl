

cbuffer TEST_B0 : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b2)
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
    float4 worldPos = mul(float4(input.pos, 1.0f), WorldMat);
    float4 viewPos = mul(worldPos, ViewMat);
    output.pos = mul(viewPos, ProjMat);
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}