#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Mesh
        {
            using gr::Graphics::Resource::VertexBuffer::VertexBuffer;
            using gr::Graphics::Resource::IndexBuffer::IndexBuffer;
            class Mesh
            {
                friend class MeshBuilder;
            public:
                std::shared_ptr<VertexBuffer> GetVertexBuffer(const std::string& key)const
                {
                    return vertexBuffers.at(key);
                }
                std::shared_ptr<IndexBuffer> GetIndexBuffer() const
                {
                    return indexBuffer;
                }
                UINT GetIndexCount() const
                {
                    return indexCount;
                }
                Mesh()
                {
                }
            private:
                std::map<std::string, std::shared_ptr<VertexBuffer>> vertexBuffers{};
                std::shared_ptr<IndexBuffer> indexBuffer{};
                UINT indexCount{};
            };

            class MeshBuilder
            {
            public:
                using Self = MeshBuilder;
                Self& CreateVertexBuffer(const std::string& key, UINT strideInBytes, UINT64 size, const void* data)
                {
                    mesh->vertexBuffers[key] = std::make_shared<VertexBuffer>(size, strideInBytes);
                    {
                        auto ptr = mesh->vertexBuffers[key]->Map();
                        memcpy(ptr, data, size);
                        mesh->vertexBuffers[key]->Unmap();
                    }
                    return *this;
                }
                Self& CreateIndexBuffer(UINT indexCount,UINT64 size,const void* data, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT)
                {
                    mesh->indexCount = indexCount;
                    mesh->indexBuffer = std::make_shared<IndexBuffer>(size, format);
                    {
                        auto ptr = mesh->indexBuffer->Map();
                        memcpy(ptr, data, size);
                        mesh->indexBuffer->Unmap();
                    }
                    return *this;
                }
                std::shared_ptr<Mesh> Build()
                {
                    return mesh;
                }
                MeshBuilder()
                {
                    mesh = std::make_shared<Mesh>();
                }
            private:
                std::shared_ptr<Mesh> mesh{};
            };
        }
    }
}