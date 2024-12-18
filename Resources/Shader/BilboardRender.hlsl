

cbuffer TEST_B0 : register(b1)
{
    row_major matrix ViewMat;
    row_major matrix ProjMat;
};

cbuffer TEST_B1 : register(b2)
{
    row_major matrix WorldMat;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState g_sam_0 : register(s0);


struct VS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;

};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(input.pos, WorldMat);
    
    output.worldPos = output.pos.xyz;
    
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    
    output.normal = normalize(mul(float4(input.normal, 0.0f), WorldMat).xyz);
    output.uv = input.uv;
   
    return output;
}

//[큐]
//[그래픽스큐] [컴퓨트큐]

//-> [] ->[] -> []
PS_OUT PS_Main(VS_OUT input)
{
  
    PS_OUT output;

    output.position = float4(input.worldPos, 1.0f);
    output.color = diffuseTexture.Sample(g_sam_0, input.uv);
    output.normal = float4(normalize(input.normal), 0.0f);
 
    float sumColor = (output.color.x + output.color.y + output.color.z) / 3.0f;
    
    clip(sumColor > 0.8f ? -1 : 1);
    
    clip(output.color.a - 0.9f);
    
    return output;

}