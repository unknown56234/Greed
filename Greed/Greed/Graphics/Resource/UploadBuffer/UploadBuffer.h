#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Resource
        {
            namespace UploadBuffer
            {
                using GfxContext = gr::Graphics::Context::Context;
                class UploadBuffer
                {
                public:
                    void* Map()
                    {
                        void* ptr{};
                        auto hr = resource->Map(0, NULL, &ptr);
                        assert(SUCCEEDED(hr));
                        return ptr;
                    }
                    void Unmap()
                    {
                        resource->Unmap(0, NULL);
                    }
                    ComPtr<ID3D12Resource> GetResource() const
                    {
                        return resource;
                    }
                    UINT64 GetSize() const
                    {
                        return size;
                    }
                    UploadBuffer(UINT64 size) :
                        size(size)
                    {
                        D3D12_HEAP_PROPERTIES heapProperties{};
                        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
                        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                        heapProperties.CreationNodeMask = 1;
                        heapProperties.VisibleNodeMask = 1;

                        D3D12_RESOURCE_DESC resourceDesc{};
                        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
                        resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
                        resourceDesc.Width = size;
                        resourceDesc.Height = 1;
                        resourceDesc.DepthOrArraySize = 1;
                        resourceDesc.MipLevels = 1;
                        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
                        resourceDesc.SampleDesc.Count = 1;
                        resourceDesc.SampleDesc.Quality = 0;
                        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


                        auto hr = GfxContext::GetInstance()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(resource.GetAddressOf()));
                        assert(SUCCEEDED(hr));
                    }
                private:
                    ComPtr<ID3D12Resource> resource{};
                    UINT64 size{};
                };
            }
        }
    }
}