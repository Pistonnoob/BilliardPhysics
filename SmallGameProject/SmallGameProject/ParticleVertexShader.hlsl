
struct VSInput
{
	float4 position : POSITION; //.a = scale
	float4 color : COLOR; //.a = rotation
};

struct GSInput
{
	float4 position : SV_POSITION; //.a = scale
	float4 color : COLOR; //.a = rotation
};

GSInput main(VSInput input)
{
	GSInput output;

	//Forward data to geo shader
	output.position = input.position;
	output.color = input.color;

	return output;
}