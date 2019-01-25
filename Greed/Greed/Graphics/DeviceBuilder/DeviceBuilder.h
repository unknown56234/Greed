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
                //VideoMemoryが一番大きいアダプタを返す
                ComPtr<IDXGIAdapter4> FindAdapter();
            };
        }
    }
}