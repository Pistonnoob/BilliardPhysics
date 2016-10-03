
cbuffer CBPerObj
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 difColor;
	float4 ambientColor;
	float4 specColor;

	float4 cameraPos;

	bool hasTexture;
};

struct VSInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
	float3 viewDir : NORMAL1;
};

GSInput main(VSInput input)
{
	GSInput output;

	//Add homogencoordinates for proper matrix multiplication
	output.position = float4(input.position, 1.0f);

	//Multiply the position with world-, view- and projectionmatrix
	//Save the world-pos of the vertex
	output.position = output.worldPos = mul(worldMatrix, output.position);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);

	//Store the color for output
	output.tex = input.tex;

	//Multiply normal with world matrix and normalize
	output.normal = normalize(mul(worldMatrix, input.normal));

	//Get the unit vector from point to camera
	output.viewDir = normalize(cameraPos.xyz - output.worldPos.xyz);

	return output;
}