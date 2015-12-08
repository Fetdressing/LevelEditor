
Texture2D txDiffuse : register(t0);
SamplerState sampAni: register(s0);


struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normals : NORMAL;
	float2 Tex : TEXCOORD;
};

float4 main(VS_OUT input) : SV_Target
{
	//float4 diffuse = txDiffuse.Sample(sampAni, input.Tex);
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
	//return diffuse;
};