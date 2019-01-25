#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace CommandAllocator
        {
            class CommandAllocator
            {
            public:
                ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const
                {
                    return commandAllocator;
                }
                void Allocate(UINT nodeMask, ComPtr<ID3D12PipelineState> initialState, const IID& riid, void** ppCommandList);
                void Reset();
                CommandAllocator(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
            private:
                ComPtr<ID3D12CommandAllocator> commandAllocator{};
                D3D12_COMMAND_LIST_TYPE type{};
            };
        }
    }
}