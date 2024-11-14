//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Color = Color;
    return output;
}

VS_OUTPUT VS_main(float4 Pos : POSITION, float4 Color : COLOR)
{
    float pi = 3.14f / 2;
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 translation = float3(1.0f, 0.3f, 1.0f);
    float3 scale = float3(0.2f, 3.0f, 3.0f);
    float4x4 rotation = float4x4(
        cos(pi), 0.0f, sin(pi), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin(pi), 0.0f, cos(pi), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    Pos.xyz += translation;
    Pos.xyz *= scale;
    Pos = mul(Pos, rotation);

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    
    output.Color = Color;
    return output;
}

VS_OUTPUT VS_main1(float4 Pos : POSITION, float4 Color : COLOR)
{
    float pi = 3.14f / 2;
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 translation = float3(-1.0f, 0.3f, -1.0f);
    float3 scale = float3(0.2f, 3.0f, 3.0f);
    float4x4 rotation = float4x4(
        cos(pi), 0.0f, sin(pi), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin(pi), 0.0f, cos(pi), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    Pos.xyz += translation;
    Pos.xyz *= scale;
    Pos = mul(Pos, rotation);

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    output.Color = Color;
    return output;
}

VS_OUTPUT VS_main2(float4 Pos : POSITION, float4 Color : COLOR)
{
    float pi = 3.14f / 2;
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 translation = float3(2.0f, 0.3f, 2.0f);
    float3 scale = float3(0.2f, 3.0f, 3.0f);
    float4x4 rotation = float4x4(
        cos(pi), 0.0f, sin(pi), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin(pi), 0.0f, cos(pi), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    Pos.xyz += translation;
    Pos.xyz *= scale;
    Pos = mul(Pos, rotation);

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    output.Color = Color;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return input.Color;
}

// New Pixel Shader 1
float4 PS1( VS_OUTPUT input ) : SV_Target
{
    // Invert the color
    return float4(1.0f - input.Color.rgb, input.Color.a);
}

// New Pixel Shader 2
float4 PS2( VS_OUTPUT input ) : SV_Target
{
    // Convert color to grayscale
    float gray = dot(input.Color.rgb, float3(0.3f, 0.59f, 0.11f));
    return float4(gray, gray, gray, input.Color.a);
}
