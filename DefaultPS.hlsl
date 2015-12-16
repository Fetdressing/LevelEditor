
Texture2D txDiffuse : register(t0);
SamplerState sampAni: register(s0);


cbuffer Material : register(b1)
{
	float diffuse;
	float3 color;
	float3 ambColor;
	float3 specColor;
	float specCosine;
	float specEccentricity;
	float specRollOff;
	float3 padding;

};

struct Light //viktigt denna ser likadan ut some på CPU sidan
{
	uint type; //0 = def, 1 = dir, 2 = spot, 3 = point
	uint decayType; //0 = none, 1 = linear, 2 = quadratic (l/d**v)
	float intensity;
	float3 colorDiffuse;
	float3 direction;
	float dropOff;
	float coneAngle;
	float penumAgle;
};

static const uint MaxLights = 128;
cbuffer Lights : register(b2)
{
	Light light[4];
	uint NumLights;
	float3 pad;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float3 ViewDirection : POSITION;
};

float4 main(VS_OUT input) : SV_Target
{
	float4 final = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 reflection = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 diffuseColor = float4(color[0], color[1], color[2], 1.0f);

	diffuseColor[0] *= diffuse;
	diffuseColor[1] *= diffuse;
	diffuseColor[2] *= diffuse;
	//diffuseColor[0] *= diffuse * lights[0].intensity;
	//diffuseColor[1] *= diffuse * lights[0].intensity;
	//diffuseColor[2] *= diffuse * lights[0].intensity;

	//for (int i = 0; i < NumLights; i++)
	//{
	//	float4 lightDir = float4(lights[i].direction.xyz, 1.0f);
	//	lightDir = -lightDir;
	//	
	//	// Calculate the amount of light on this pixel.
	//	float lightIntensity = lights[i].intensity;
	//	lightIntensity = saturate(dot(input.Normal, lightDir));
	//	
	//	if (lightIntensity > 0.0f)
	//	{
	//		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	//		final += (diffuseColor * lightIntensity);

	//		// Saturate the ambient and diffuse color.
	//		final = saturate(final);

	//		//The reflection vector for specular lighting is calculated here in the pixel shader provided the light intensity is greater than zero.This is the same equation as listed at the beginning of the tutorial.

	//		//	// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
	//		reflection = normalize(2 * lightIntensity * input.Normal - lightDir);

	//		//The amount of specular light is then calculated using the reflection vector and the viewing direction.The smaller the angle between the viewer and the light source the greater the specular light reflection will be.The result is taken to the power of the specularPower value.The lower the specularPower value the greater the final effect is.

	//		//	// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	//		specular = pow(saturate(dot(reflection, input.ViewDirection)), 0.5f);
	//	}

	//	//// Multiply the texture pixel and the input color to get the textured result.
	//	//final = final * textureColor;

	//	// Add the specular component last to the output color.
	//	diffuseColor = saturate(diffuseColor + specular);
	//}
	float4 diffuse = txDiffuse.Sample(sampAni, input.Tex);
	return diffuse;
	//return diffuse;
};