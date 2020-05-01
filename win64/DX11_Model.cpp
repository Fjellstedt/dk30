/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "w_pch.h"
#include "DX11_Model.h"
#include "DX11_Macros.h"
namespace Cryptic
{
    DX11_Model::DX11_Model()
    {
        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
        m_vertexCount = 0;
        m_indexCount = 0;
    }

    DX11_Model::~DX11_Model()
    {
    }

    bool DX11_Model::Initialize(ID3D11Device *device, U32 vertexCount, U32 indexCount)
    {
        m_vertexCount = vertexCount;
        m_indexCount = indexCount;

        // TODO(pf): REMOVE THIS, generating models should be done with a temp. memory stack later on.
        VertexLayout *vertices = new VertexLayout[m_vertexCount];
        unsigned long *indices = new unsigned long[m_indexCount];

        vertices[0].pos = DirectX::XMFLOAT3(-1.f, -1.f, 0.f);
        vertices[0].tex = DirectX::XMFLOAT2(0.f, 1.0f);
        vertices[0].normal = DirectX::XMFLOAT3(0.f, 0.f, -1.0f);

        vertices[1].pos = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
        vertices[1].tex = DirectX::XMFLOAT2(0.5f, 0.f);
        vertices[1].normal = DirectX::XMFLOAT3(0.f, 0.f, -1.0f);

        vertices[2].pos = DirectX::XMFLOAT3(1.f, -1.f, 0.f);
        vertices[2].tex = DirectX::XMFLOAT2(1.f, 1.f);
        vertices[2].normal = DirectX::XMFLOAT3(0.f, 0.f, -1.0f);
        
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(VertexLayout) * m_vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA vertexData;
        vertexData.pSysMem = vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;
        DX_HR(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer), L"DX11: Failed to create a vertex buffer.");

        D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexData;
        indexData.pSysMem = indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;
        DX_HR(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer), L"DX11: Failed to create a index buffer.");

        // TODO(PF): REMOVE THIS...
        delete[] vertices;
        delete[] indices;

        return true;
    }

    void DX11_Model::Shutdown()
    {
        DX_RELEASE(m_vertexBuffer);
        DX_RELEASE(m_indexBuffer);
    }

    void DX11_Model::Render(ID3D11DeviceContext *context)
    {
        U32 stride = sizeof(VertexLayout), offset = 0;
        
        context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);/*DXGI_FORMAT_R8G8B8A8_UINT*/
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}