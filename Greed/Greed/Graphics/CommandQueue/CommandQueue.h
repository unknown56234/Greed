#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace CommandQueue
        {
            class CommandQueue
            {
            public:
                ComPtr<ID3D12CommandQueue> GetCommandQueue() const
                {
                    return commandQueue;
                }
                BOOL WaitIdle();
                CommandQueue(ComPtr<ID3D12Device> device, const D3D12_COMMAND_QUEUE_DESC& desc);
            private:
                UINT fenceValue{};
                ComPtr<ID3D12CommandQueue> commandQueue{};
                ComPtr<ID3D12Fence> fence{};
            };
        }
    }
}