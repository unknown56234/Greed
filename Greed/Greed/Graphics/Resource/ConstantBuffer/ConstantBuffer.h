#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Resource
        {
            namespace ConstantBuffer
            {
                using GfxContext = gr::Graphics::Context::Context;
                using gr::Graphics::Resource::UploadBuffer::UploadBuffer;
                class ConstantBuffer :public UploadBuffer
                {
                public:
                    ConstantBuffer(UINT64 size, D3D12_CPU_DESCRIPTOR_HANDLE handle):
                        UploadBuffer(size + 0xff & ~0xff/*256アライメントにする*/)
                    {
                        auto alignedSize = size + 0xff & ~0xff/*256アライメントにする*/;
                        D3D12_CONSTANT_BUFFER_VIEW_DESC bufferViewDesc{};
                        bufferViewDesc.BufferLocation = GetResource()->GetGPUVirtualAddress();
                        bufferViewDesc.SizeInBytes = static_cast<UINT>(alignedSize);
                        GfxContext::GetInstance()->GetDevice()->CreateConstantBufferView(&bufferViewDesc, handle);

                    }
                };
            }
        }
    }
}