#include "../../Common/Common.h"
#include "../Graphics.h"

namespace gr
{
    namespace Graphics
    {
        namespace CommandQueue
        {
            BOOL CommandQueue::WaitIdle()
            {
                if (FAILED(commandQueue->Signal(fence.Get(), fenceValue))) return FALSE;

                if (fence->GetCompletedValue() < fenceValue)
                {
                    HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
                    if (FAILED(fence->SetEventOnCompletion(fenceValue, fenceEvent))) return FALSE;
                    WaitForSingleObject(fenceEvent, INFINITE);
                    CloseHandle(fenceEvent);
                }
                fenceValue++;
                return TRUE;
            }

            CommandQueue::CommandQueue(ComPtr<ID3D12Device> device, const D3D12_COMMAND_QUEUE_DESC & desc)
            {
                device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
                device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
                fenceValue = 1;
            }
        }
    }
}
