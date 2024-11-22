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

    float pi = 3.14f / 2;

    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 translation = float3(2.0f, 0.0f, -3.0);
    float3 scale = float3(0.25f, 0.25f, 0.25f);

    float4 materialAmb = float4(0.1, 0.2, 0.2, 1.0);
    float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);
    float4 lightCol = float4(1.0, 0.6, 0.8, 1.0);
    float3 lightDir = normalize(LightPos.xyz - Pos.xyz); // Corrected variable name//float3(-2.0f, 12.0f, -10.0f)
    float3 normal = normalize(N);
    float diff = max(0.0, dot(lightDir, normal));

    
    Pos.xyz += translation;
    Pos.xyz *= scale;
 

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    
    output.Color = diff*float4(lightDir, 1);// (materialAmb + diff * materialDiff)* lightCol;
    output.PosWorld = Pos.xyz;
    output.Norm = N.xyz;

    return output;
}
