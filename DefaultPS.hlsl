
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

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normals : NORMAL;
	float2 Tex : TEXCOORD;
};

float4 main(VS_OUT input) : SV_Target
{
	float4 diffuseColor = float4(color[0], color[1], color[2], 1.0f);
	diffuseColor[0] *= diffuse;
	diffuseColor[1] *= diffuse;
	diffuseColor[2] *= diffuse;
	//float4 diffuse = txDiffuse.Sample(sampAni, input.Tex);
	return diffuseColor;
	//return diffuse;
};