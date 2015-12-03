//VERTEX SHADER
cbuffer World : register (b10)
{
	matrix View;
	matrix Projection;
	matrix WorldSpace;
	matrix WorldSpaceInv;

	matrix lightView;
	matrix lightProjection;
};

cbuffer World : register (b0)
{
	matrix World;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 normals : NORMAL;

};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 normals : NORMAL;
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

	//inputpos = mul(inputpos, World);
	output.Pos = inputpos;
	output.Tex = input.Tex;
	output.normals = float4(input.normals, 1.0f);

	return output;
}
