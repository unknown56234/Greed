#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Context
        {
            using gr::Graphics::DeviceBuilder::DeviceBuilder;
            using gr::Graphics::CommandQueue::CommandQueue;
            using gr::Graphics::CommandAllocator::CommandAllocator;

            class Context
            {
            public:
                ComPtr<ID3D12Device>& GetDevice()
                {
                    return device;
                }
                std::unique_ptr<CommandQueue>& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
                {
                    return commandQueues.at(type);
                }
                std::shared_ptr<CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
                {
                    return std::make_shared<CommandAllocator>(device, type);
                }
                static std::unique_ptr<Context>& GetInstance()
                {
                    return instance;
                }
                static void CreateInstance()
                {
                    instance = std::make_unique<Context>(Context{});
                }
                static void DestroyInstance()
                {
                    instance = nullptr;
                }
            private:
                Context();
                void CreateQueue(D3D12_COMMAND_LIST_TYPE type);

                ComPtr<ID3D12Device> device{};
                std::map<D3D12_COMMAND_LIST_TYPE, std::unique_ptr<CommandQueue>> commandQueues{};

                static std::unique_ptr<Context> instance;
            };
        }
    }
}