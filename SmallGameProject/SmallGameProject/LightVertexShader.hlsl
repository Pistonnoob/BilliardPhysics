cbuffer LightConstantBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;

	float4 camPos;
	float4 lightPos;
};

struct VSInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
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
	output.position = mul(viewMatrix, input.position);
	output.position = mul(projectionMatrix, output.position);

	//Store the uv for output
	output.tex = input.tex;

	//output will be in clip space of the light projection plane
	return output;
}