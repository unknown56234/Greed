#pragma once
namespace gr
{
    namespace Common
    {
        namespace Resource
        {
            namespace ImageLoader
            {
                namespace WICLoader
                {
                    using gr::Common::Resource::ImageLoader::ImageLoaderBase::ImageLoaderBase;
                    class WICLoader :public ImageLoaderBase
                    {
                    public:
                        virtual DirectX::ScratchImage Load(std::filesystem::path path) override
                        {
                            DirectX::ScratchImage scratchImage{};
                            DirectX::LoadFromWICFile(path.wstring().c_str(), 0, nullptr, scratchImage);
                            return scratchImage;
                        }
                    };
                }
            }
        }
    }
}