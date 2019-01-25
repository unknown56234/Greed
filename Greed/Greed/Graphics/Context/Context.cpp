#include"../../Common/Common.h"
#include"../Graphics.h"

namespace gr
{
    namespace Graphics
    {
        namespace Context
        {
            std::unique_ptr<Context> Context::instance{};
            Context::Context()
            {
                device = DeviceBuilder().Build();
                CreateQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
                CreateQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
                CreateQueue(D3D12_COMMAND_LIST_TYPE_COPY);
                CreateQueue(D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE);
                CreateQueue(D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS);
            }
            void Context::CreateQueue(D3D12_COMMAND_LIST_TYPE type)
            {
                D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
                commandQueueDesc.Type = type;
                commandQueueDesc.Priority = 0;
                commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                commandQueueDesc.NodeMask = 0;

                commandQueues[type] = std::make_unique<CommandQueue>(device, commandQueueDesc);
            }
        }
    }
}