struct VS_IN
{
    float3 localPos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD;
};

cbuffer CameraParams : register(b1)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
};

cbuffer TransformParams : register(b2)
{
    row_major matrix WorldMatrix;
};

TextureCube g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 로컬 좌표를 그대로 전달
    output.localPos = input.localPos;
    
    // Translation은 하지 않고 Rotation만 적용한다
    float4 viewPos = mul(float4(input.localPos, 0), ViewMatrix);
    float4 clipSpacePos = mul(viewPos, ProjectionMatrix);

    // w/w=1이기 때문에 항상 깊이가 1로 유지된다
    output.pos = clipSpacePos.xyww;
    

    return output;
}


struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};


PS_OUT PS_Main(VS_OUT input)
{    
    PS_OUT output;
    
    output.position = input.pos;
    output.normal = float4(0, 0.1f, 0.1f, 0);
    output.color = g_tex_0.Sample(g_sam_0, input.localPos);
    
    return output;
}