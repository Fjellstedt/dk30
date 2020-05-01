cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct Vert_In
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct Pixel_In
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

Pixel_In Test_Vertex(Vert_In input)
{
	Pixel_In result;
	input.pos.w = 1.0f;

	result.pos = mul(input.pos, worldMatrix);
	result.pos = mul(result.pos, viewMatrix);
	result.pos = mul(result.pos, projectionMatrix);

	result.tex = input.tex;

	result.normal = mul(input.normal, (float3x3)worldMatrix);
	result.normal = normalize(result.normal);

	return result;
}