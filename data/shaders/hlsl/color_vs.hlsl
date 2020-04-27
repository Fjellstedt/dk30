
cbuffer
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

Pixel_In Color_Vert(Vert_In input)
{
	Pixel_In result;
	result.position.w = 1.0f;

	result.position = mul(mul(mul(input.pos, worldMatrix), viewMatrix), projectionMatrix);
	result.color = input.color;

	return result;
}