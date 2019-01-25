#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Resource
        {
            namespace IndexBuffer
            {
                using GfxContext = gr::Graphics::Context::Context;
                using gr::Graphics::Resource::UploadBuffer::UploadBuffer;
                class IndexBuffer :public UploadBuffer
                {
                public:
                    const D3D12_INDEX_BUFFER_VIEW& GetBufferView() const
                    {
                        return bufferView;
                    }
                    IndexBuffer(UINT64 size, DXGI_FORMAT format=DXGI_FORMAT_R32_UINT) :
                        UploadBuffer(size)
                    {
                        bufferView.BufferLocation = GetResource()->GetGPUVirtualAddress();
                        bufferView.SizeInBytes = static_cast<UINT>(size);
                        bufferView.Format = format;
                    }
                private:
                    D3D12_INDEX_BUFFER_VIEW bufferView{};
                };
            }
        }
    }
}