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
        m_data = nullptr;
    }

    DX11_Model::~DX11_Model()
    {
    }

    bool DX11_Model::Initialize(ID3D11Device *device, ModelData *data, MemoryStack *frameMemory)
    {
        m_data = data;

        MemoryMarker stackMarker = frameMemory->currentSize;
        VertexLayout *vertices = frameMemory->Allocate<VertexLayout>(m_data->vertexCount);
        unsigned long *indices = frameMemory->Allocate<unsigned long>(m_data->indexCount);

        for (int i = 0; i < (I32)m_data->vertexCount; ++i)
        {
            vertices[i].pos = DirectX::XMFLOAT3(m_data->layout[i].x, m_data->layout[i].y, m_data->layout[i].z);
            vertices[i].tex = DirectX::XMFLOAT2(m_data->layout[i].tu, m_data->layout[i].tv);
            vertices[i].normal = DirectX::XMFLOAT3(m_data->layout[i].nx, m_data->layout[i].ny, m_data->layout[i].nz);
            indices[i] = i;
        }

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(VertexLayout) * m_data->vertexCount;
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
        indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_data->indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexData;
        indexData.pSysMem = indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;
        DX_HR(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer), L"DX11: Failed to create a index buffer.");

        frameMemory->ResetToMarker(stackMarker);

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