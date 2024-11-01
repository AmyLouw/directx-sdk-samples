//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
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
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4x4 ScaleMatrix = float4x4(0.2, 0.0, 0.0, 0.0,
        0.0, 0.3, 0.0, 0.0,
        0.0, 0.0, 3.0, 0.0,
        0.0, 0.0, 0.0, 1.0);
    float angle = radians(45.0); //rotation angle
    float4x4 RotationMatrix = float4x4(cos(angle), 0.0, sin(angle), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sin(angle), 0.0, cos(angle), 0.0,
        0.0, 0.0, 0.0, 1.0);
    Pos = mul(Pos, ScaleMatrix);
    Pos = mul(Pos, RotationMatrix);
    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = Color;
    return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)  
//{  
    //VS_OUTPUT output = (VS_OUTPUT)0;  
    //float4x4 ScaleMatrix = float4x4(0.2, 0.0, 0.0, 0.0,  
                                    //0.0, 3.0, 0.0, 0.0,  
                                    //0.0, 0.0, 3.0, 0.0,  
                                    //0.0, 0.0, 0.0, 1.0);  
                                    
    //float angle = radians(45.0); // Example rotation angle  
    //float4x4 RotationMatrix = float4x4(cos(angle), 0.0, sin(angle), 0.0,  
                                      //0.0, 1.0, 0.0, 0.0,  
                                       //-sin(angle), 0.0, cos(angle), 0.0,  
                                       //0.0, 0.0, 0.0, 1.0);  
                                       
    //Pos = mul(Pos, ScaleMatrix);  
    //Pos = mul(Pos, RotationMatrix);  
    //float4 inPos = Pos;
    //output.Pos = mul(output.Pos, View);  
    //output.Pos = mul(output.Pos, Projection);  
    //output.Color = Color;  
    //return output;  
//}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}

