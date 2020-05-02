Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
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


float4 Test_Pixel(Pixel_In input): SV_TARGET
{
	float4 result;
float4 textureColor = (shaderTexture.Sample(sampleType, input.tex));
	float lightIntensity = max(saturate(dot(input.normal, -lightDir)), 0);
	float4 lightColor = ambientColor;
	lightColor = saturate(lightColor + (lightIntensity * diffuseColor));
	result = textureColor * lightColor;

	return result;
}