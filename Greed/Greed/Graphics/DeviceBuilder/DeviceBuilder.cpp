#include"../../Common/Common.h"
#include"../Graphics.h"

namespace gr
{
    namespace Graphics
    {
        namespace DeviceBuilder
        {
            ComPtr<ID3D12Device> DeviceBuilder::Build()
            {
                auto adapter = FindAdapter();
                ComPtr<ID3D12Device> device{};
                auto hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf()));
                assert(SUCCEEDED(hr));
                return device;
            }
            ComPtr<IDXGIAdapter4> DeviceBuilder::FindAdapter()
            {
                ComPtr<IDXGIAdapter4> dxgiAdapter{};

                ComPtr<IDXGIFactory4> dxgiFactory{};
                ComPtr<IDXGIAdapter1> dxgiAdapter1{};
                UINT dxgiFlag = 0;
#ifdef _DEBUG
                dxgiFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif
                SIZE_T maxDedicatedVideoMemory{};
                CreateDXGIFactory2(dxgiFlag, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
                for (int i = 0; dxgiFactory->EnumAdapters1(i, dxgiAdapter1.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++)
                {
                    DXGI_ADAPTER_DESC1 adapterDesc1;
                    dxgiAdapter1->GetDesc1(&adapterDesc1);
                    //softwareのアダプタは使わない
                    if ((adapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) continue;
                    //device作れるか確認
                    if (FAILED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) continue;
                    if (adapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
                    {
                        HRESULT hr = dxgiAdapter1.As(&dxgiAdapter);
                        assert(SUCCEEDED(hr));
                        maxDedicatedVideoMemory = adapterDesc1.DedicatedVideoMemory;
                    }
                }
                return dxgiAdapter;
            }
        }
    }
}