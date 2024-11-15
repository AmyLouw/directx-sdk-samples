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

float4 PS_main(VS_OUTPUT input) : SV_Target
{
	return float4(0,0,1,1);
}
