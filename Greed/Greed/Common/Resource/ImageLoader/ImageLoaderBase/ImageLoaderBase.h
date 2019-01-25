#pragma once
namespace gr
{
    namespace Common
    {
        namespace Resource
        {
            namespace ImageLoader
            {
                namespace ImageLoaderBase
                {
                    class ImageLoaderBase
                    {
                    public:
                        virtual DirectX::ScratchImage Load(std::filesystem::path path) = 0;

                    private:
                    };
                }
            }
        }
    }
}