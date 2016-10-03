cbuffer CBPerEmitter
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 camPos;
};

struct GSInput
{
	float4 position : SV_POSITION; //.a = scale
	float4 color : COLOR; //.a = rotation
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

[maxvertexcount(6)]
void main(point GSInput input[1], inout TriangleStream< PSInput > output)
{
	PSInput element;
	float size = input[0].position.w;
	/*float hSize = size / 2;
	float4 centerWorld = float4(input[0].position.x + hSize, input[0].position.y + hSize, input[0].position.z, 1.0f);
*/
	input[0].position.w = 1.0f;
	////float4 center = float4(input[0].position.x + (size / 2), input[0].position.y + (size / 2), input[0].position.z, 1.0f);
	//float4 worldPos = mul(centerWorld, worldMatrix);

	////Create roation matrix
	//float3 look = normalize(worldPos.xyz - camPos.xyz);

	//float3 camUp = float3(0.0f, 1.0f, 0.0f);

	//float3 right = normalize(cross(look, camUp));

	//float3 up = normalize(cross(look, right));

	/*float4x4 rotationMatrix = {
		right.x, up.x, look.x, worldPos.x,
		right.y, up.y, look.y, worldPos.y,
		right.z, up.z, look.z, worldPos.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	float4x4 transRotMatrix = transpose(rotationMatrix);*/
	/*float4x4 rotationMatrix = {
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		look.x, look.y, look.z, 0.0f,
		worldPos.x, worldPos.y, worldPos.z, 1.0f
	};*/

	/*float4x4 rotationMatrix;
	rotationMatrix[0] = float4(right, 0.0f);
	rotationMatrix[1] = float4(up, 0.0f);
	rotationMatrix[2] = float4(look, 0.0f);
	rotationMatrix[3] = float4(worldPos.xyz, 1.0f);*/

	/*float4 topLeft = float4(-right * size + up * size, 1.0f);
	float4 topRight = float4(right * size + up * size, 1.0f);
	float4 bottomLeft = float4(-right * size - up * size, 1.0f);
	float4 bottomRight = float4(right * size - up * size, 1.0f);*/

	//**Create quad**\\

	float4 viewPos = mul(worldMatrix, input[0].position);
	viewPos = mul(viewMatrix, viewPos);

	//*First triangle*\\
	//Top-Left
	element.position = float4(viewPos.x - size, viewPos.y + size, viewPos.z, viewPos.w);;
	element.position = mul(projectionMatrix, element.position);
	element.color = input[0].color;
	element.tex = float2(input[0].color.a, 0.0f);
	output.Append(element);
	//Bottom-Right
	element.position = float4(viewPos.x + size, viewPos.y - size, viewPos.z, viewPos.w);
	element.position = mul(projectionMatrix, element.position);
	element.color = input[0].color;
	element.tex = float2(input[0].color.a + 0.25f, 1.0f);
	output.Append(element);
	//Bottom-Left
	element.position = float4(viewPos.x - size, viewPos.y - size, viewPos.z, viewPos.w);
	element.position = mul(projectionMatrix, element.position);
	element.color = input[0].color;
	element.tex = float2(input[0].color.a, 1.0f);
	output.Append(element);
	output.RestartStrip();

	//*Second triangle*\\
	//Top-Left
	element.position = float4(viewPos.x - size, viewPos.y + size, viewPos.z, viewPos.w);;
	element.position = mul(projectionMatrix, element.position);
	element.color = input[0].color;
	element.tex = float2(input[0].color.a, 0.0f);
	output.Append(element);
	//Top-Right
	element.position = float4(viewPos.x + size, viewPos.y + size, viewPos.z, viewPos.w);
	element.position = mul(projectionMatrix, element.position);
	element.color = input[0].color;
	element.tex = float2(input[0].color.a + 0.25f, 0.0f);
	output.Append(element);
	//Bottom-Right
	element.position = float4(viewPos.x + size, viewPos.y - size, viewPos.z, viewPos.w);
	element.position = mul(projectionMatrix, element.position);
	element.color = input[0].color;
	element.tex = float2(input[0].color.a + 0.25f, 1.0f);
	output.Append(element);
	output.RestartStrip();

	////**Create quad**\\

	////*First triangle*\\
	////Top-Left
	//element.position = input[0].position;
	////element.position = input[0].position + topLeft;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, rotationMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(0.0f, 0.0f);
	//output.Append(element);
	////Bottom-Right
	//element.position = float4(input[0].position.x + size, input[0].position.y - size, input[0].position.z, 1.0f);
	////element.position = input[0].position + bottomRight;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, rotationMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(1.0f, 1.0f);
	//output.Append(element);
	////Bottom-Left
	//element.position = float4(input[0].position.x, input[0].position.y - size, input[0].position.z, 1.0f);
	////element.position = input[0].position + bottomLeft;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, rotationMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(0.0f, 1.0f);
	//output.Append(element);
	//output.RestartStrip();

	////*Second triangle*\\
	////Top-Left
	//element.position = input[0].position;
	////element.position = input[0].position + topLeft;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, rotationMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(0.0f, 0.0f);
	//output.Append(element);
	////Top-Right
	//element.position = float4(input[0].position.x + size, input[0].position.y, input[0].position.z, 1.0f);
	////element.position = input[0].position + topRight;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, rotationMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(1.0f, 0.0f);
	//output.Append(element);
	////Bottom-Right
	//element.position = float4(input[0].position.x + size, input[0].position.y - size, input[0].position.z, 1.0f);
	////element.position = input[0].position + bottomRight;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, rotationMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(1.0f, 1.0f);
	//output.Append(element);
	//output.RestartStrip();

	////**Create quad without rotation**\\

	////*First triangle*\\
	////Top-Left
	//element.position = input[0].position;
	////element.position = input[0].position + topLeft;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, worldMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(0.0f, 0.0f);
	//output.Append(element);
	////Bottom-Right
	//element.position = float4(input[0].position.x + size, input[0].position.y - size, input[0].position.z, 1.0f);
	////element.position = input[0].position + bottomRight;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, worldMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(1.0f, 1.0f);
	//output.Append(element);
	////Bottom-Left
	//element.position = float4(input[0].position.x, input[0].position.y - size, input[0].position.z, 1.0f);
	////element.position = input[0].position + bottomLeft;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, worldMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(0.0f, 1.0f);
	//output.Append(element);
	//output.RestartStrip();

	////*Second triangle*\\
	////Top-Left
	//element.position = input[0].position;
	////element.position = input[0].position + topLeft;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, worldMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(0.0f, 0.0f);
	//output.Append(element);
	////Top-Right
	//element.position = float4(input[0].position.x + size, input[0].position.y, input[0].position.z, 1.0f);
	////element.position = input[0].position + topRight;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, worldMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(1.0f, 0.0f);
	//output.Append(element);
	////Bottom-Right
	//element.position = float4(input[0].position.x + size, input[0].position.y - size, input[0].position.z, 1.0f);
	////element.position = input[0].position + bottomRight;
	////element.position.w = 1.0f;
	//element.position = mul(element.position, worldMatrix);
	//element.position = mul(element.position, viewMatrix);
	//element.position = mul(element.position, projectionMatrix);
	//element.color = input[0].color;
	//element.tex = float2(1.0f, 1.0f);
	//output.Append(element);
	//output.RestartStrip();
}