cbuffer FontConstantBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float3 color;
};

struct VSInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PSInput main(VSInput input)
{
	PSInput output;

	//Add homogencoordinates for proper matrix multiplication
	input.position.w = 1.0f;

	//Multiply the position with world-, view- and projectionmatrix
	output.position = mul(worldMatrix, input.position);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);

	//Store the color for output
	output.tex = input.tex;

	return output;
}