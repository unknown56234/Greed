#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace DeviceBuilder
        {
            class DeviceBuilder
            {
            public:
                ComPtr<ID3D12Device> Build();
            private:
                //VideoMemory����ԑ傫���A�_�v�^��Ԃ�
                ComPtr<IDXGIAdapter4> FindAdapter();
            };
        }
    }
}