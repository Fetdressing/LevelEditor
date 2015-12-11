//VERTEX SHADER
cbuffer Camera : register (b10)
{
	matrix View;
	matrix Projection;
	float4 CameraPosition;
};

cbuffer World : register (b0)
{
	matrix World;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;

};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float3 ViewDirection : POSITION;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 inputpos = float4(input.Pos, 1.0f);

	float4 worldPosition = mul(inputpos, World);
	inputpos = mul(worldPosition, View);
	inputpos = mul(inputpos, Projection);

	output.Pos = inputpos;
	output.Normal = float4(input.Normal, 1.0f);
	output.Tex = input.Tex;

	// Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
	output.ViewDirection = CameraPosition.xyz - worldPosition.xyz;
	// Normalize the viewing direction vector.
	output.ViewDirection = normalize(output.ViewDirection);

	return output;
}
