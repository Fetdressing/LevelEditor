//VERTEX SHADER
cbuffer Camera : register (b3)
{
	matrix View;
	matrix Projection;	
};

cbuffer World : register (b0)
{
	matrix World;
};

//cbuffer Material : register(b1)
//{
//
//};

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Normals : NORMAL;
	float2 Tex : TEXCOORD;

};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normals : NORMAL;
	float2 Tex : TEXCOORD;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 inputpos = float4(input.Pos, 1.0f);
		// Change the position vector to be 4 units for proper matrix calculations.
		// Calculate the position of the vertex against the world, view, and projection matrices.
	//inputpos = mul(mul(mul(inputpos, WorldSpace),View), Projection);
	//inputpos = mul(inputpos, WorldSpace);
	//inputpos = mul(inputpos, View);
	//inputpos = mul(inputpos, Projection);

	inputpos = mul(inputpos, World);
	//inputpos = mul(inputpos, View);
	//inputpos = mul(inputpos, Projection);

	output.Pos = inputpos;
	output.Normals = float4(input.Normals, 1.0f);
	output.Tex = input.Tex;

	return output;
}
