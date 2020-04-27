struct Pixel_In
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 Color_Pixel(Pixel_In input)
{
	float4 result = input.color;
	return result;
}