#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace DebugLayer
        {
            class DebugLayer
            {
            public:
                static void Enable()
                {
                    ComPtr<ID3D12Debug> debugController{};
                    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
                    {
                        debugController->EnableDebugLayer();
                    }
                }
            };
        }
    }
}