#pragma once
#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Resource
        {
            namespace VertexBuffer
            {
                using GfxContext = gr::Graphics::Context::Context;
                using gr::Graphics::Resource::UploadBuffer::UploadBuffer;

                class VertexBuffer :public UploadBuffer
                {
                public:
                    const D3D12_VERTEX_BUFFER_VIEW& GetBufferView() const
                    {
                        return bufferView;
                    }
                    VertexBuffer(UINT64 size, UINT strideInBytes) :
                        UploadBuffer(size)
                    {
                        bufferView.BufferLocation = GetResource()->GetGPUVirtualAddress();
                        bufferView.SizeInBytes = static_cast<UINT>(size);
                        bufferView.StrideInBytes = strideInBytes;
                    }
                private:
                    D3D12_VERTEX_BUFFER_VIEW bufferView{};
                };
            }
        }
    }
}