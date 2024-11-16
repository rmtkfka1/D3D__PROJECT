
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
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 uv: TEXCOORD;
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
    
    output.pos = float4(input.pos, 1.f);
   
    return output;
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
   
    float hw = 5.0f;
    GS_OUT output;
    float2 uvCoords[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    float2 offsets[4] = { float2(-hw, hw), float2(hw, hw), float2(-hw, -hw), float2(hw, -hw) };

    float3 normal = float3(0, 0, 1.0f);

    // 첫 번째 삼각형
    output.pos = input[0].pos;
    output.pos.xy += offsets[0];
    output.pos = mul(output.pos, WorldMat);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = uvCoords[0];
    output.normal = normal;
    outputStream.Append(output);
    
    output.pos = input[0].pos;
    output.pos.xy += offsets[1];
    output.pos = mul(output.pos, WorldMat);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = uvCoords[1];
    output.normal = normal;
    outputStream.Append(output);
    
    output.pos = input[0].pos;
    output.pos.xy += offsets[2];
    output.pos = mul(output.pos, WorldMat);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = uvCoords[2];
    output.normal = normal;
    outputStream.Append(output);

    // 두 번째 삼각형 (새로운 스트립을 시작하지 않고 연결)
    outputStream.RestartStrip();
    
    output.pos = input[0].pos;
    output.pos.xy += offsets[2];
    output.pos = mul(output.pos, WorldMat);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = uvCoords[2];
    output.normal = normal;
    outputStream.Append(output);
    
    output.pos = input[0].pos;
    output.pos.xy += offsets[1];
    output.pos = mul(output.pos, WorldMat);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = uvCoords[1];
    output.normal = normal;
    outputStream.Append(output);
    
    output.pos = input[0].pos;
    output.pos.xy += offsets[3];
    output.pos = mul(output.pos, WorldMat);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, ViewMat);
    output.pos = mul(output.pos, ProjMat);
    output.uv = uvCoords[3];
    output.normal = normal;
    outputStream.Append(output);
   
}


PS_OUT PS_Main(GS_OUT input)
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