cbuffer CameraMatrixBuffer : register(b10)
{
	matrix View;
	matrix Projection;
	float4 CameraPosition;
};

cbuffer World : register (b0)
{
	matrix World;
};

struct VertexInputType
{
    float3 position : POSITION;
	float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct VOut
{
    float4 position : SV_POSITION;
	float3 normal: NORMAL;
	float2 texCoord : TEXCOORD;
    float4 worldPos : WORLDPOS;
};

VOut VS_main(VertexInputType input, uint instanceID : SV_InstanceID)
{
    VOut output;
    output.position = mul(float4(input.position, 1.0f), World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);



    output.worldPos = mul(float4(input.position, 1.0f), World);
    output.normal = mul(float4(input.normal, 0.0f), World);
    output.texCoord = input.texCoord;
    //output.cameraPos = cameraPosition;
    return output;
}