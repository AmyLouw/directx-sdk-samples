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
	float3 PosWorld : TEXCOORD1;
	float3 Norm : TEXCOORD2;
};

float4 PS_main(VS_OUTPUT input) : SV_Target
{

	/*float3 normalizedNormal = normalize(input.Norm);
	float3 lightDir = normalize(float3(1.0, 1.0, 1.0));
	float diff = max(0.0, dot(normalizedNormal, lightDir));
	float4 diffuseColour = float4(1.0,0.0,0.0,1.0) * diff;*/
    return input.Color;
}