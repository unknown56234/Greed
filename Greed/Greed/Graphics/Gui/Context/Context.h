#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Gui
        {
            namespace Context
            {
                class Context
                {
                public:
                    void NewFrame() const
                    {
                        ImGui_ImplDX12_NewFrame();
                        ImGui_ImplWin32_NewFrame();
                        ImGui::NewFrame();
                    }
                    void Render(ComPtr<ID3D12GraphicsCommandList> commandList) const
                    {
                        auto heap = descriptorHeap.Get();
                        commandList->SetDescriptorHeaps(1, &heap);
                        ImGui::Render();
                        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
                    }
                    Context(HWND hWnd, int numFramesInFlight)
                    {
                        auto& device = gr::Graphics::Context::Context::GetInstance()->GetDevice();
                        IMGUI_CHECKVERSION();
                        ImGui::CreateContext();
                        ImGuiIO& io = ImGui::GetIO(); (void)io;

                        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
                        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                        descriptorHeapDesc.NumDescriptors = 1;
                        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                        descriptorHeapDesc.NodeMask = 0;
                        device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));

                        ImGui_ImplWin32_Init(hWnd);
                        ImGui_ImplDX12_Init(device.Get(), numFramesInFlight,
                            DXGI_FORMAT_R8G8B8A8_UNORM,
                            descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                            descriptorHeap->GetGPUDescriptorHandleForHeapStart());

                        ImGui::StyleColorsDark();
                    }
                    ~Context()
                    {
                        ImGui_ImplDX12_Shutdown();
                        ImGui_ImplWin32_Shutdown();
                        ImGui::DestroyContext();
                    }
                private:
                    ComPtr<ID3D12DescriptorHeap> descriptorHeap{};
                };
            }
        }
    }
}