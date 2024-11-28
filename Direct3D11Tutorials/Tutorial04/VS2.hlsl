cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 LightPos; // Corrected variable name
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float3 PosWorld : TEXCOORD1;
    float3 Norm : TEXCOORD2;
};


VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR, float3 N : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    //float3 translation = float3(2.0f, 0.0f, -3.0);
    float3 scale = float3(0.25f, 0.25f, 0.25f);

    //Pos.xyz += translation;
    Pos.xyz *= scale;

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = mul(float4(N, 0), World).xyz; // Fix the normal calculation
    output.PosWorld = output.Pos;

    return output;
}
