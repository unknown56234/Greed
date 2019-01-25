#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace SwapChain
        {
            class SwapChain
            {
            public:
                ComPtr<IDXGISwapChain3> GetSwapchain() const
                {
                    return swapChain;
                }
                UINT GetWidth() const
                {
                    return width;
                }
                UINT GetHeight() const
                {
                    return height;
                }
                //RenderTarget�̃o���A���ւ��A�Z�b�g�A�N���A�܂ł��R�}���h���X�g�ɋL�^����
                //DepthStencil�̃N���A�͂��Ȃ��̂ŊO���ł���K�v������

                UINT BeginRender(ComPtr<ID3D12GraphicsCommandList> commandList, float r, float g, float b, float a, D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDescriptor = NULL);
                UINT BeginRender(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDescriptor = NULL);
                //RenderTarget�̃o���A���ւ����R�}���h���X�g�ɋL�^����
                void EndRender(ComPtr<ID3D12GraphicsCommandList> commandList);
                void Resize(ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd, UINT width, UINT height);
                SwapChain(HWND hWnd, UINT backBufferCount);
            private:
                void CreateSwapchain(ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd);
                void CreateDescriptorHeap(ComPtr<ID3D12Device> device);
                void CreateRenderTargetViews(ComPtr<ID3D12Device> device);
                void DestroyRenderTargets();

                UINT backBufferCount{};
                UINT backBufferIndex{};
                ComPtr<IDXGISwapChain3> swapChain{};
                HANDLE waitableObject{};
                ComPtr<ID3D12DescriptorHeap> backBufferDescriptorHeap{};
                std::vector<ComPtr<ID3D12Resource>> backBuffers{};
                std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> backBufferHandle{};

                UINT width{};
                UINT height{};
            };
        }
    }
}