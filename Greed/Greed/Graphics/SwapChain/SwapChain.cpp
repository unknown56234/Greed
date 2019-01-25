#include"../../Common/Common.h"
#include"../Graphics.h"
namespace gr
{
    namespace Graphics
    {
        namespace SwapChain
        {
            UINT SwapChain::BeginRender(ComPtr<ID3D12GraphicsCommandList> commandList, float r, float g, float b, float a, D3D12_CPU_DESCRIPTOR_HANDLE * depthStencilDescriptor)
            {
                {
                    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

                    D3D12_RESOURCE_BARRIER	resourceBarrier = {};
                    resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    resourceBarrier.Transition.pResource = backBuffers[backBufferIndex].Get();
                    resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                    resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                    resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    commandList->ResourceBarrier(1, &resourceBarrier);

                    commandList->OMSetRenderTargets(1, &backBufferHandle[backBufferIndex], FALSE, depthStencilDescriptor);
                    const float	clearColor[] = { r, g, b, a };

                    commandList->ClearRenderTargetView(backBufferHandle[backBufferIndex], clearColor, 0, NULL);
                    return backBufferIndex;
                }
            }
            UINT SwapChain::BeginRender(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_CPU_DESCRIPTOR_HANDLE * depthStencilDescriptor)
            {
                return BeginRender(commandList, 0.0f, 0.0f, 0.0f, 1.0f, depthStencilDescriptor);
            }

            //RenderTargetのバリア張替えをコマンドリストに記録する

            void SwapChain::EndRender(ComPtr<ID3D12GraphicsCommandList> commandList)
            {
                D3D12_RESOURCE_BARRIER	resourceBarrier = {};
                resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                resourceBarrier.Transition.pResource = backBuffers[backBufferIndex].Get();
                resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
                resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                commandList->ResourceBarrier(1, &resourceBarrier);
            }
            void SwapChain::Resize(ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd, UINT width, UINT height)
            {
                //対応したウィンドウかどうか判別
                HWND hCurrentWindow{};
                swapChain->GetHwnd(&hCurrentWindow);
                if (hWnd != hCurrentWindow)
                {
                    return;
                }

                this->width = width;
                this->height = height;

                DestroyRenderTargets();

                HRESULT hr{};

                DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
                hr = swapChain->GetDesc1(&swapChainDesc);
                assert(SUCCEEDED(hr));

                swapChainDesc.Width = width;
                swapChainDesc.Height = height;

                ComPtr<IDXGIFactory4> dxgiFactory{};
                swapChain->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
                swapChain = nullptr;
                CloseHandle(waitableObject);

                ComPtr<IDXGISwapChain1> swapChain1{};
                hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, NULL, NULL, swapChain1.GetAddressOf());
                assert(SUCCEEDED(hr));
                hr = swapChain1->QueryInterface(IID_PPV_ARGS(swapChain.ReleaseAndGetAddressOf()));
                assert(SUCCEEDED(hr));

                swapChain->SetMaximumFrameLatency(backBufferCount);
                waitableObject = swapChain->GetFrameLatencyWaitableObject();

                ComPtr<ID3D12Device> device{};
                swapChain->GetDevice(IID_PPV_ARGS(device.GetAddressOf()));

                CreateDescriptorHeap(device);
                CreateRenderTargetViews(device);
            }
            SwapChain::SwapChain(HWND hWnd, UINT backBufferCount) :
                backBufferCount(backBufferCount)
            {
                auto& device = gr::Graphics::Context::Context::GetInstance()->GetDevice();
                auto& commandQueue = gr::Graphics::Context::Context::GetInstance()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
                CreateSwapchain(commandQueue->GetCommandQueue(), hWnd);
                CreateDescriptorHeap(device);
                CreateRenderTargetViews(device);
            }
            void SwapChain::CreateSwapchain(ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd)
            {
                HRESULT hr{};

                DXGI_SWAP_CHAIN_DESC1 sd{};
                sd.BufferCount = backBufferCount;
                sd.Width = 0;
                sd.Height = 0;
                sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
                sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                sd.SampleDesc.Count = 1;
                sd.SampleDesc.Quality = 0;
                sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
                sd.Scaling = DXGI_SCALING_STRETCH;
                sd.Stereo = FALSE;

                UINT dxgiFlag = 0;
#ifdef _DEBUG
                dxgiFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif

                ComPtr<IDXGIFactory4> dxgiFactory{};
                ComPtr<IDXGISwapChain1> swapChain1{};
                hr = CreateDXGIFactory2(dxgiFlag, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
                assert(SUCCEEDED(hr));

                hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &sd, NULL, NULL, swapChain1.GetAddressOf());
                assert(SUCCEEDED(hr));

                hr = swapChain1->QueryInterface(IID_PPV_ARGS(swapChain.GetAddressOf()));
                assert(SUCCEEDED(hr));

                swapChain->SetMaximumFrameLatency(backBufferCount);
                waitableObject = swapChain->GetFrameLatencyWaitableObject();
            }
            void SwapChain::CreateDescriptorHeap(ComPtr<ID3D12Device> device)
            {
                HRESULT hr{};
                D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
                descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                descriptorHeapDesc.NumDescriptors = backBufferCount;
                descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                descriptorHeapDesc.NodeMask = 0;
                hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(backBufferDescriptorHeap.ReleaseAndGetAddressOf()));
            }
            void SwapChain::CreateRenderTargetViews(ComPtr<ID3D12Device> device)
            {
                backBuffers.resize(backBufferCount);
                backBufferHandle.resize(backBufferCount);

                HRESULT hr{};

                auto backBufferDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                for (unsigned int i = 0; i < backBufferCount; i++)
                {
                    hr = swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffers[i].ReleaseAndGetAddressOf()));
                    assert(SUCCEEDED(hr));
                    backBufferHandle[i] = backBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                    backBufferHandle[i].ptr += backBufferDescriptorSize * i;
                    device->CreateRenderTargetView(backBuffers[i].Get(), NULL, backBufferHandle[i]);
                }
            }
            void SwapChain::DestroyRenderTargets()
            {
                backBufferDescriptorHeap = nullptr;
                for (auto&& backBuffer : backBuffers)
                {
                    backBuffer = nullptr;
                }
            }
        }
    }
}