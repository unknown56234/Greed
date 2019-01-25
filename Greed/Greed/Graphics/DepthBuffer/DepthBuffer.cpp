#include"../../Common/Common.h"
#include"../Graphics.h"

namespace gr
{
    namespace Graphics
    {
        namespace DepthBuffer
        {
            DepthBuffer::DepthBuffer(UINT width, UINT height, DXGI_FORMAT format) :
                format(format)
            {
                auto& device = gr::Graphics::Context::Context::GetInstance()->GetDevice();
                CreateDescriptorHeap(device);
                CreateResource(device, width, height, format);
                CreateView(device);
            }
            void DepthBuffer::CreateDescriptorHeap(ComPtr<ID3D12Device> device)
            {
                D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
                descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
                descriptorHeapDesc.NumDescriptors = 1;
                descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                descriptorHeapDesc.NodeMask = 0;
                device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
            }
            void DepthBuffer::CreateResource(ComPtr<ID3D12Device> device, UINT width, UINT height, DXGI_FORMAT format)
            {
                D3D12_HEAP_PROPERTIES heapProperties{};
                heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
                heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                heapProperties.CreationNodeMask = 1;
                heapProperties.VisibleNodeMask = 1;

                D3D12_RESOURCE_DESC resourceDesc{};
                resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
                resourceDesc.Width = width;
                resourceDesc.Height = height;
                resourceDesc.DepthOrArraySize = 1;
                resourceDesc.MipLevels = 0;
                resourceDesc.Format = format;
                resourceDesc.SampleDesc.Count = 1;
                resourceDesc.SampleDesc.Quality = 0;
                resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

                D3D12_CLEAR_VALUE clearValue{};
                clearValue.Format = format;
                clearValue.DepthStencil.Depth = 1.0f;
                clearValue.DepthStencil.Stencil = 0;

                device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(buffer.GetAddressOf()));
            }
            void DepthBuffer::CreateView(ComPtr<ID3D12Device> device)
            {
                D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
                depthStencilViewDesc.Format = format;
                depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

                device->CreateDepthStencilView(buffer.Get(), &depthStencilViewDesc, descriptorHeap->GetCPUDescriptorHandleForHeapStart());
            }
        }
    }
}