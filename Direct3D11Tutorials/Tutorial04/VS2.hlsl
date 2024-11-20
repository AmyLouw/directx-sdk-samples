
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix LightPos;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};


VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR, float3 N : NORMAL)
{

    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 translation = float3(0.0f, 1.0f, 0.0);
    float3 scale = float3(1.5f, 1.5f, 1.5f);
    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);


    
    Pos.xyz += translation;
    Pos.xyz *= scale;
 

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    float3 lightDir = normalize(lightPos.xyz - Pos.xyz);
    float3 normal = normalize(N);
    float diff = max(0.0, dot(lightDir, normal));
    output.Color = (materialAmb + diff * materialDiff) * lightCol;

    output.Color =  Color;
    return output;
}
