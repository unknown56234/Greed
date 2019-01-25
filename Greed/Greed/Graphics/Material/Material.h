#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Material
        {
            using GfxContext = gr::Graphics::Context::Context;
            using gr::Graphics::Resource::Image::Image;
            using gr::Graphics::Resource::ConstantBuffer::ConstantBuffer;
            using DirectX::XMFLOAT4;
            struct MaterialValues
            {
                XMFLOAT4 baseColorFactor{};
                float metallic{};
                float roughness{};
            };
            class Material
            {
                friend class MaterialBuilder;
            public:
                ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const
                {
                    return descriptorHeap;
                }
                Material()
                {

                }
            private:
                std::shared_ptr<Image> baseColorImage{};
                std::shared_ptr<Image> metallicRoughnessImage{};
                std::shared_ptr<Image> normalImage{};
                std::shared_ptr<ConstantBuffer> materialValues{};
                ComPtr<ID3D12DescriptorHeap> descriptorHeap{};
            };

            class MaterialBuilder
            {
            public:
                using Self = MaterialBuilder;
                Self& SetBaseColorImage(std::shared_ptr<Image> image)
                {
                    material->baseColorImage = image;
                    return *this;
                }
                Self& SetMetallicRoughnessImage(std::shared_ptr<Image> image)
                {
                    material->metallicRoughnessImage = image;
                    return *this;
                }
                Self& SetNormalImage(std::shared_ptr<Image> image)
                {
                    material->normalImage = image;
                    return *this;
                }
                Self& SetBaseColorFactor(const XMFLOAT4& color)
                {
                    materialValues.baseColorFactor = color;
                    return *this;
                }
                Self& SetMetallic(float metallic)
                {
                    materialValues.metallic = metallic;
                    return *this;
                }
                Self& SetRoughness(float roughness)
                {
                    materialValues.roughness = roughness;
                    return *this;
                }
                std::shared_ptr<Material> Build()
                {
                    auto& device = GfxContext::GetInstance()->GetDevice();
                    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
                    descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                    descriptorHeapDesc.NumDescriptors = 8;
                    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    descriptorHeapDesc.NodeMask = 0;
                    device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(material->descriptorHeap.GetAddressOf()));

                    auto handle = material->descriptorHeap->GetCPUDescriptorHandleForHeapStart();
                    auto incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
                    shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;    //Œã‚Å“ü‚ê‚é
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    shaderResourceViewDesc.Texture2D.MipLevels = 1;
                    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
                    shaderResourceViewDesc.Texture2D.PlaneSlice = 0;
                    shaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                    shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

                    shaderResourceViewDesc.Format = material->baseColorImage->GetFormat();
                    device->CreateShaderResourceView(material->baseColorImage->GetResource().Get(), &shaderResourceViewDesc, handle);
                    handle.ptr += incrementSize;

                    shaderResourceViewDesc.Format = material->metallicRoughnessImage->GetFormat();
                    device->CreateShaderResourceView(material->metallicRoughnessImage->GetResource().Get(), &shaderResourceViewDesc, handle);
                    handle.ptr += incrementSize;

                    shaderResourceViewDesc.Format = material->normalImage->GetFormat();
                    device->CreateShaderResourceView(material->normalImage->GetResource().Get(), &shaderResourceViewDesc, handle);
                    handle.ptr += incrementSize;

                    material->materialValues = std::make_shared<ConstantBuffer>(sizeof(MaterialValues), handle);

                    {
                        auto ptr = material->materialValues->Map();
                        memcpy(ptr, &materialValues, sizeof(MaterialValues));
                        material->materialValues->Unmap();
                    }
                    return material;
                }
                MaterialBuilder()
                {
                    material = std::make_shared<Material>();
                }
            private:
                std::shared_ptr<Material> material{};
                MaterialValues materialValues{};
            };
        }
    }
}