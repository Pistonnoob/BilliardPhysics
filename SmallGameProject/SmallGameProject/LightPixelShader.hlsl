#define MAX_LIGHTS 10
#define SCENE_AMBIENT 0.1f

Texture2D diffTexture : register(t0);
Texture2D ambientTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D normalTexture: register(t3);
Texture2D worldPosTexture: register(t4);
Texture2D shadowMapTexture: register(t5);

SamplerState pointSampler : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightMatrixBuffer : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;

	float4 camPos;
};

cbuffer LightsCB : register(b1)
{
	float4 Ambient[MAX_LIGHTS];
	float4 Diffuse[MAX_LIGHTS];
	float4 Specular[MAX_LIGHTS];

	float4 Position[MAX_LIGHTS];
	float4 Attenuation[MAX_LIGHTS-1];

	uint activeLights;
	float3 padding;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 diffColor;
	float4 ambientColor;
	float4 specColor;

	float4 normal;
	float4 worldPos;
	float4 outputColor;
	float4 outputAmbient;
	float2 textCoords = float2(input.position.x / 800, input.position.y / 600);
	float2 shadowUV;
	float4 positionLight;
	float lightDepthValue;
	float lightIntensity = 0.0f;
	float shadowCoeff = 1;

	float depthValue = 0;

	//float4 lightPos = (10.0f, 10.0f, 0.0f, 1.0f);

	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.00002f;

	//Sample the diffuse texture from deferred render
	diffColor = diffTexture.Sample(pointSampler, textCoords);

	//Sample the ambient texture from deferred render
	ambientColor = ambientTexture.Sample(pointSampler, textCoords);

	//Sample the specual texture from deferred render
	specColor = specularTexture.Sample(pointSampler, textCoords);

	//Sample the normal texture from deferred render
	normal = normalTexture.Sample(pointSampler, textCoords);
	
	//Sample the texture with positions in world space from deferred render
	worldPos = worldPosTexture.Sample(pointSampler, textCoords);

	float3 outVec = normalize(-Position[0]);

	//Move the position to projection space for the light
	positionLight = mul(lightViewMatrix, worldPos);
	positionLight = mul(lightProjectionMatrix, positionLight);

	//calculate the projected texture coordinate
	shadowUV.x = (positionLight.x / positionLight.w) * 0.5f + 0.5f;
	shadowUV.y = (positionLight.y / positionLight.w) * -0.5f + 0.5f;


	//If point outside shadowMap
	if (saturate(shadowUV.x) != shadowUV.x || saturate(shadowUV.y) != shadowUV.y)
	{
		lightIntensity = saturate(dot(normal.xyz, outVec.xyz));
	}
	else
	{
		//pixel is in shadow
		lightIntensity = 0;

		//calculate the depth of the light
		lightDepthValue = positionLight.z / positionLight.w;

		//sample the shadowmap
		depthValue = shadowMapTexture.Sample(shadowSampler, shadowUV).r;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;


		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(normal.xyz, outVec.xyz));
			if (lightIntensity < 0) {
				lightIntensity = 0;
			}
		}

	}

	//Create the normalized reflection vector
	float3 refVec = normalize(reflect(-outVec, normal));

	//Creathe the normalized vector from position to camera
	float3 viewDir = normalize(camPos - worldPos).xyz;

	float specIntesity = saturate(dot(refVec, viewDir));
	float shineFactor = specColor.a;
	float lightSpecular = Specular[0];

	//Calculate the specular part
	float4 specular = float4(specColor.rgb * lightSpecular * max(pow(specIntesity, shineFactor), 0.0f), 1.0f);

	outputColor = saturate(((diffColor.rgba * Diffuse[0]) + (specular.rgba * Specular[0])) * lightIntensity);
	outputAmbient = ((ambientColor.rgba * Ambient[0]));

	for (int i = 1; i <= activeLights; i++) {
		outputAmbient += saturate(ambientColor.rgba * Ambient[i]);
		//Create the normalized vector from position to light source
		outVec = Position[i].xyz - (worldPos).xyz;
		float distToLight = length(outVec);
		if (distToLight <= Attenuation[i - 1].x) {
			outVec = normalize(outVec);
			//Create the normalized reflection vector
			refVec = normalize(reflect(-outVec, normal));

			specIntesity = saturate(dot(refVec, viewDir));
			lightSpecular = Specular[i];

			//Calculate the specular part
			specular = float4(specColor.rgb * lightSpecular * max(pow(specIntesity, shineFactor), 0.0f), 1.0f);

			lightIntensity = saturate(dot(normal, outVec));
			if (lightIntensity < 0) {
				lightIntensity = 0;
			}

			float atten = Attenuation[i - 1].y + Attenuation[i - 1].z * distToLight + Attenuation[i - 1].w * distToLight * distToLight;
			float lumen = (1.0f / atten);
			lightIntensity = lightIntensity * lumen;

			outputColor += ((diffColor.rgba * Diffuse[i]) + (specular.rgba * Specular[i])) * lightIntensity;
		}
	}
	outputAmbient = saturate(outputAmbient) * SCENE_AMBIENT;
	outputColor = saturate(outputColor + outputAmbient);
	//saturate(lightIntensity);
	//Create the normalized vector from position to light source
	////float3 outVec = normalize(lightPos.xyz - (worldPos).xyz);
	//float3 outVec = normalize(-Position[0]);

	////Create the normalized reflection vector
	//float3 refVec = normalize(reflect(-outVec, normal));

	////Creathe the normalized vector from position to camera
	//float3 viewDir = normalize(camPos - worldPos).xyz;

	//float specIntesity = saturate(dot(refVec, viewDir));
	//float shineFactor = 5.0f;
	//float lightSpecular = 0.65f;

	////Calculate the specular part
	//float4 specular = float4(specColor.rgb * lightSpecular * max(pow(specIntesity, shineFactor), 0.0f), 1.0f);

	//lightIntensity = dot(normal, outVec);
	//if (lightIntensity < 0) {
	//	lightIntensity = 0;
	//}

	//Combine everything for the output color
	//outputColor = saturate(((diffColor.rgba + specular.rgba) * lightIntensity * 0.8f) + ((ambientColor.rgba) * 0.2f));

	return outputColor;
}