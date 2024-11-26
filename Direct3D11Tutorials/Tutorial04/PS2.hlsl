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
	float3 PosWorld : TEXCOORD1;
	float3 Norm : TEXCOORD2;
};

float4 PS_main(VS_OUTPUT input) : SV_Target
{

	float4 mateiralAmb = float4(0.1,0.2,0.2,1.0);
	float4 materialDiff = float4(0.9,0.7,1.0,1.0);
	float4 lightCol = float4(1.0,0.6,0.8,1.0);
	float3 lightDir = float3(-2.0f,12.0f,-10.0f);
	float3 normal = normalize(input.Norm);
	float diff = max(0.0,dot(lightDir,normal));
	
    output.Color = (materialAmb + diff * materialDiff) * lightCol;
	
    return input;
}