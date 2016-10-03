Texture2D fontTexture : register(t0);
SamplerState mySampler : register(s0);

cbuffer FontConstantBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float3 color;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 outputColor = fontTexture.Sample(mySampler, input.tex);
	//If pixel is black, treat it as transparent
	if (outputColor.r == 1) {
		outputColor.a = 0.0f;
	}
	else {
		outputColor.a = 1.0f;
	}

	outputColor.xyz = outputColor * color.xyz;

	return outputColor;
}