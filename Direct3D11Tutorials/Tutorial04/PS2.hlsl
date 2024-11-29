cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 LightPos;
	float3 CameraPos;
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
	float4 materialAmb = float4(0.1, 0.2, 0.5, 1.0);
	float4 materialDiff = float4(0.9, 0.7, 1.0, 1.0);

	float4 materialSpec = float4(1.0, 1.0, 1.0, 1.0); 

	float4 lightCol = float4(1.0, 1.0, 1.0, 1.0);

	float3 lightDir = normalize(LightPos.xyz - input.PosWorld.xyz);
	//float3 debugColor = float3(abs(lightDir.x), abs(lightDir.y), abs(lightDir.z));

	float3 normal = normalize(input.Norm);
	float diff = max(0.0, dot(lightDir, normal));
	float3 reflectedDir = reflect(-lightDir, input.Norm);
	float3 viewDir = normalize(CameraPos - input.PosWorld);

	float spec = pow(max(0.0, dot(viewDir, reflectedDir)), 2);

	float4 SpecColor = (1 * lightCol * spec);

	float4 color = SpecColor + ((materialAmb + diff * materialDiff) * lightCol);



	
	//return float4(debugColor, 1.0);

	return color;
}