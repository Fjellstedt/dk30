Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDir;
	float padding;
};

struct Pixel_In
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 Test_Pixel(Pixel_In input) : SV_TARGET
{
	float4 result = (shaderTexture.Sample(sampleType, input.tex));

	float lightIntensity = saturate(dot(input.normal, -lightDir));
	float4 lightColor = saturate(lightIntensity * diffuseColor);
	result = result * lightColor;

	return result;
}