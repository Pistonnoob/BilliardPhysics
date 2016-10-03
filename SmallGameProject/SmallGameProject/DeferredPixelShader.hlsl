Texture2D shaderTexture;
SamplerState shaderSampler;

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

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

struct PSOutput
{
	float4 color : SV_Target0;
	float4 ambient : SV_Target1;
	float4 specular : SV_Target2;
	float4 normal : SV_Target3;
	float4 worldPos : SV_Target4;
};

PSOutput main(PSInput input)
{
	PSOutput output;

	if (hasTexture) {
		output.color = shaderTexture.Sample(shaderSampler, input.tex).rgba;
		output.ambient = output.color;
	}
	else {
		output.color = difColor;
		output.ambient = ambientColor;
	}
	output.specular = specColor;

	output.normal = float4(input.normal, 1.0f);

	output.worldPos = input.worldPos;

	return output;
}