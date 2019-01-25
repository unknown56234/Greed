#pragma once
#include"ImageLoaderBase/ImageLoaderBase.h"
#include"DDSLoader/DDSLoader.h"
#include"WICLoader/WICLoader.h"
namespace gr
{
    namespace Common
    {
        namespace Resource
        {
            namespace ImageLoader
            {
                class ImageLoader
                {
                public:
                    DirectX::ScratchImage Load(std::filesystem::path path)
                    {
                        auto ext = path.extension().string();
                        if (loaders.find(path.extension().string()) == loaders.end())
                        {
                            panic("image format is not supported");
                        }
                        return loaders[path.extension().string()]->Load(path);
                    }
                    ImageLoader()
                    {
                        loaders[".dds"] = std::make_shared<DDSLoader::DDSLoader>();
                        loaders[".png"] = std::make_shared<WICLoader::WICLoader>();
                        loaders[".jpg"] = loaders[".png"];
                    }
                private:
                    std::map<std::string, std::shared_ptr<ImageLoaderBase::ImageLoaderBase>> loaders{};
                };
            }
        }
    }
}