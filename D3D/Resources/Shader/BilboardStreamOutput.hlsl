
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
    float3 pos : POSITION; //12
    float2 uv: TEXCOORD ;  //8
    float3 normal : NORMAL;   //12   
    float3 tangent : TANGENT; //12   
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = float4(input.pos, 1.f);
   
    return output;
}

[maxvertexcount(4)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
   
    float hw = 5.0f;
    GS_OUT output;
    float2 uvCoords[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    float2 offsets[4] = { float2(-hw, hw), float2(hw, hw), float2(-hw, -hw), float2(hw, -hw) };

    float3 normal = float3(0, 0, 1.0f);

    for (int i = 0; i < 4; ++i)
    {
        output.pos = input[0].pos;
        output.pos.xy += offsets[i]; 
        
        output.uv = uvCoords[i];
        output.normal = normal;
        output.tangent = normal;

        outputStream.Append(output);
    }
   
 
}


