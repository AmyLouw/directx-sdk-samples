
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};


VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR)
{
    float pi = 3.14f / 2;
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 translation = float3(-1.0f, -1.0f, 2.0f);
    float3 scale = float3(1.0f, 0.5f, 0.5f);
    Pos.xyz += translation;
    Pos.xyz *= scale;

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.Color = Color;
    return output;
}
