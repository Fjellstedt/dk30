cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct Vert_In
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct Pixel_In
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

Pixel_In Color_Vertex(Vert_In input)
{
	Pixel_In result;
	input.pos.w = 1.0f;

	result.pos = mul(input.pos, worldMatrix);
	result.pos = mul(result.pos, viewMatrix);
	result.pos = mul(result.pos, projectionMatrix);
	result.color = input.color;

	return result;
}