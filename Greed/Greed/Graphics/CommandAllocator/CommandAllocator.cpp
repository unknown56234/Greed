#include "../../Common/Common.h"
#include "../Graphics.h"

namespace gr
{
    namespace Graphics
    {
        namespace CommandAllocator
        {
            void CommandAllocator::Allocate(UINT nodeMask, ComPtr<ID3D12PipelineState> initialState, const IID & riid, void ** ppCommandList)
            {
                ComPtr<ID3D12Device> device{};
                commandAllocator->GetDevice(IID_PPV_ARGS(device.GetAddressOf()));
                device->CreateCommandList(nodeMask, type, commandAllocator.Get(), initialState.Get(), riid, ppCommandList);
            }

            void CommandAllocator::Reset()
            {
                commandAllocator->Reset();
            }

            CommandAllocator::CommandAllocator(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type) :
                type(type)
            {
                device->CreateCommandAllocator(type, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
            }
        }
    }
}
