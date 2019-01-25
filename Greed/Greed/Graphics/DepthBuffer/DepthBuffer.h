#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace DepthBuffer
        {
            class DepthBuffer
            {
            public:
                ComPtr<ID3D12Resource> GetBuffer() const
                {
                    return buffer;
                }
                ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const
                {
                    return descriptorHeap;
                }
                DXGI_FORMAT GetFormat() const
                {
                    return format;
                }
                DepthBuffer(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);
            private:
                void CreateDescriptorHeap(ComPtr<ID3D12Device> device);
                void CreateResource(ComPtr<ID3D12Device> device, UINT width, UINT height, DXGI_FORMAT format);
                void CreateView(ComPtr<ID3D12Device> device);

                ComPtr<ID3D12DescriptorHeap> descriptorHeap{};
                ComPtr<ID3D12Resource> buffer{};
                DXGI_FORMAT format{};
            };
        }
    }
}