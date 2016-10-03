Texture2D particleTexture : register(t0);
SamplerState mySampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 outputColor = particleTexture.Sample(mySampler, input.tex);

	outputColor.rgb = outputColor.rgb * input.color.rgb;
	outputColor.a *= 0.4f;

	return outputColor;
}