/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "w_pch.h"
#include "DX11_Shader.h"
#include "DX11_Macros.h"
#include <fstream>

namespace Cryptic
{
	DX11_Shader::DX11_Shader()
	{
		m_vertexShader = nullptr;
		m_pixelShader = nullptr;
		m_layout = nullptr;
		m_matrixBuffer = nullptr;
		m_lightBuffer = nullptr;
		m_samplerState = nullptr;
	}

	DX11_Shader::~DX11_Shader()
	{
	}

	B32 DX11_Shader::Initialize(ID3D11Device *device, HWND hwnd, const wchar_t *vsFileName, const wchar_t *psFileName)
	{
		ID3D10Blob *errorMessage = nullptr;
		ID3D10Blob *vertexShaderBuffer;
		ID3D10Blob *pixelShaderBuffer;
		HRESULT hr = D3DCompileFromFile(vsFileName, 0, 0, "Test_Vertex", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
										&vertexShaderBuffer, &errorMessage);
		if (hr)
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
				return false;
			}
			else
				DX_HR(hr, L"DX11: Failed to load vertex shader.");
		}

		hr = D3DCompileFromFile(psFileName, 0, 0, "Test_Pixel", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
								&pixelShaderBuffer, &errorMessage);
		if (hr)
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
				return false;
			}
			else
				DX_HR(hr, L"DX11: Failed to load pixel shader.");
		}

		DX_HR(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
										 0, &m_vertexShader), L"DX11: Failed to create vertex shader buffer");

		DX_HR(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
										0, &m_pixelShader), L"DX11: Failed to create pixelshader buffer");

		D3D11_INPUT_ELEMENT_DESC polygonLayout[3] = {};
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		polygonLayout[2].SemanticName = "NORMAL";
		polygonLayout[2].SemanticIndex = 0;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[2].InputSlot = 0;
		polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[2].InstanceDataStepRate = 0;

		unsigned int numElements = ArrayCount(polygonLayout);
		DX_HR(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(), &m_layout), L"DX11: Failed to create an input layout.");
		DX_RELEASE(vertexShaderBuffer);
		DX_RELEASE(pixelShaderBuffer);

		D3D11_BUFFER_DESC matrixBufferDesc = {};
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferLayout);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		DX_HR(device->CreateBuffer(&matrixBufferDesc, 0, &m_matrixBuffer), L"DX11: Failed to create layout matrix buffer.");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.MipLODBias = 0.0f;
		sampDesc.MaxAnisotropy = 1;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.BorderColor[0] = 0;
		sampDesc.BorderColor[1] = 0;
		sampDesc.BorderColor[2] = 0;
		sampDesc.BorderColor[3] = 0;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX_HR(device->CreateSamplerState(&sampDesc, &m_samplerState), L"DX11: Failed to create sampler state.");

		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(LightBufferLayout);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = 0;
		lightBufferDesc.StructureByteStride = 0;

		DX_HR(device->CreateBuffer(&lightBufferDesc, 0, &m_lightBuffer), L"DX11: Failed to create shader light buffer");

		return true;
	}

	void DX11_Shader::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, const wchar_t *shaderFilename)
	{
		// Get a pointer to the error message text buffer.
		char *compileErrors = (char *)(errorMessage->GetBufferPointer());

		// Get the length of the message.
		SIZE_T bufferSize = errorMessage->GetBufferSize();

		// Open a file to write the error message to.
		std::ofstream fout;
		fout.open("shader-error.txt");

		// Write out the error message.
		for (SIZE_T i = 0; i < bufferSize && compileErrors[i] != '\0'; i++)
		{
			fout << compileErrors[i];
		}
		// Close the file.
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = 0;

		// Pop a message up on the screen to notify the user to check the text file for compile errors.
		MessageBoxW(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

		return;
	}

	void DX11_Shader::Shutdown()
	{
		DX_RELEASE(m_vertexShader);
		DX_RELEASE(m_pixelShader);
		DX_RELEASE(m_layout);
		DX_RELEASE(m_matrixBuffer);
		DX_RELEASE(m_lightBuffer);
		DX_RELEASE(m_samplerState);
	}

	B32 DX11_Shader::Render(ID3D11DeviceContext *context, int indexCount, ID3D11ShaderResourceView *texture,
							DirectX::XMFLOAT4 diffuseLight, DirectX::XMFLOAT3 lightDir,
							DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
	{
		// NOTE(pf): First we setup the shader resource cbuffer variables..
		world = DirectX::XMMatrixTranspose(world);
		view = DirectX::XMMatrixTranspose(view);
		projection = DirectX::XMMatrixTranspose(projection);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		DX_HR(context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
			  L"DX11: Failed to write matrix data to shader.");

		MatrixBufferLayout *matrixShaderData = (MatrixBufferLayout *)mappedResource.pData;
		matrixShaderData->world = world;
		matrixShaderData->view = view;
		matrixShaderData->projection = projection;
		context->Unmap(m_matrixBuffer, 0);

		DX_HR(context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
			  L"DX11: Failed to write light data to shader.");
		LightBufferLayout *lightShaderData = (LightBufferLayout *)mappedResource.pData;
		lightShaderData->diffuseColor = diffuseLight;
		lightShaderData->lightDir = lightDir;
		lightShaderData->padding = 0.0f;
		context->Unmap(m_lightBuffer, 0);

		context->PSSetConstantBuffers(0, 1, &m_lightBuffer);

		context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
		context->PSSetShaderResources(0, 1, &texture);

		// NOTE(pf): .. then we render the shader.
		context->IASetInputLayout(m_layout);
		context->VSSetShader(m_vertexShader, 0, 0);
		context->PSSetShader(m_pixelShader, 0, 0);
		context->PSSetSamplers(0, 1, &m_samplerState);
		context->DrawIndexed(indexCount, 0, 0);

		return true;
	}
}