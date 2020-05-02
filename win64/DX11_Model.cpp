/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#include "w_pch.h"
#include "DX11_Model.h"
#include "DX11_Macros.h"
#include <fstream>

namespace Cryptic
{
    DX11_Model::DX11_Model()
    {
        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
        m_modelLayout = nullptr;
        m_vertexCount = 0;
        m_indexCount = 0;
    }

    DX11_Model::~DX11_Model()
    {
    }

    bool DX11_Model::Initialize(ID3D11Device *device, const char *fileName, MemoryStack *memory)
    {
        if (!LoadModel(fileName, memory))
        {
            return false;
        }
        // TODO(pf): REMOVE THIS, generating models should be done with a temp. memory stack later on.
        VertexLayout *vertices = new VertexLayout[m_vertexCount];
        unsigned long *indices = new unsigned long[m_indexCount];

        for (int i = 0; i < (I32)m_vertexCount; ++i)
        {
            vertices[i].pos = DirectX::XMFLOAT3(m_modelLayout[i].x, m_modelLayout[i].y, m_modelLayout[i].z);
            vertices[i].tex = DirectX::XMFLOAT2(m_modelLayout[i].tu, m_modelLayout[i].tv);
            vertices[i].normal = DirectX::XMFLOAT3(m_modelLayout[i].nx, m_modelLayout[i].ny, m_modelLayout[i].nz);
            indices[i] = i;
        }

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

    B32 DX11_Model::LoadModel(const char *fileName, MemoryStack *memory)
    {
        std::ifstream fin;
        fin.open(fileName);
        if (fin.fail())
        {
            return false;
        }

        char input = {};
        while (input != ':') { fin.get(input); }
        fin >> m_vertexCount;
        input = {};
        while (input != ':') { fin.get(input); }
        fin >> m_indexCount;
        input = {};

        m_modelLayout = memory->Allocate<ModelLayout>(m_vertexCount);
        input = {};
        while (input != ':') { fin.get(input); }
        fin.get();
        fin.get();
        for (int i = 0; i < (I32)m_vertexCount; ++i)
        {
            fin >> m_modelLayout[i].x >> m_modelLayout[i].y >> m_modelLayout[i].z;
            fin >> m_modelLayout[i].tu >> m_modelLayout[i].tv;
            fin >> m_modelLayout[i].nx >> m_modelLayout[i].ny >> m_modelLayout[i].nz;
        }
        fin.close();
        return true;
    }
}