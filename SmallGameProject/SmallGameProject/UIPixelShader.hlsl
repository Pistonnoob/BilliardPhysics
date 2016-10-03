Texture2D uiTexture : register(t0);
SamplerState mySampler : register(s0);

cbuffer UIConstantBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 outputColor = uiTexture.Sample(mySampler, input.tex);

	return outputColor;
}