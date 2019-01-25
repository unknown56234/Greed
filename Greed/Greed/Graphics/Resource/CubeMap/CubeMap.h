#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Resource
        {
            namespace CubeMap
            {
                using GfxContext = gr::Graphics::Context::Context;
                using gr::Graphics::Resource::UploadBuffer::UploadBuffer;
                class CubeMap
                {
                public:
                    ComPtr<ID3D12Resource> GetResource() const
                    {
                        return resource;
                    }
                    DXGI_FORMAT GetFormat() const
                    {
                        return resource->GetDesc().Format;
                    }
                    CubeMap(
                        ComPtr<ID3D12CommandAllocator> commandAllocator,
                        ComPtr<ID3D12GraphicsCommandList> commandList,
                        const std::array<DirectX::ScratchImage, 6>& images
                    )
                    {
                        HRESULT hr{};
                        D3D12_HEAP_PROPERTIES heapProperties{};
                        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
                        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                        heapProperties.CreationNodeMask = 1;
                        heapProperties.VisibleNodeMask = 1;

                        D3D12_RESOURCE_DESC resourceDesc{};
                        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                        resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
                        resourceDesc.Width = images[0].GetMetadata().width;
                        resourceDesc.Height = static_cast<UINT>(images[0].GetMetadata().height);
                        resourceDesc.DepthOrArraySize = 6;
                        resourceDesc.MipLevels = 1;
                        resourceDesc.Format = images[0].GetMetadata().format;
                        resourceDesc.SampleDesc.Count = 1;
                        resourceDesc.SampleDesc.Quality = 0;
                        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

                        hr = GfxContext::GetInstance()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(resource.GetAddressOf()));

                        for (int i = 0; i < 6; i++)
                        {
                            D3D12_PLACED_SUBRESOURCE_FOOTPRINT  footprint{};
                            UINT64  total_bytes{};
                            GfxContext::GetInstance()->GetDevice()->GetCopyableFootprints(&resourceDesc, i, 1, 0, &footprint, nullptr, nullptr, &total_bytes);

                            auto stagingBuffer = std::make_unique<UploadBuffer>(total_bytes);
                            {
                                auto ptr = reinterpret_cast<unsigned char*>(stagingBuffer->Map());
                                ptr += footprint.Offset;
                                memcpy(ptr, images[i].GetPixels(), images[i].GetPixelsSize());
                                stagingBuffer->Unmap();
                            }

                            D3D12_TEXTURE_COPY_LOCATION dst{};
                            dst.pResource = resource.Get();
                            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                            dst.SubresourceIndex = i;

                            D3D12_TEXTURE_COPY_LOCATION src{};
                            src.pResource = stagingBuffer->GetResource().Get();
                            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                            src.PlacedFootprint = footprint;

                            commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
                            commandList->Close();
                            std::vector<ID3D12CommandList*> commands{ commandList.Get() };
                            auto& commandQueue = GfxContext::GetInstance()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
                            commandQueue->GetCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commands.size()), commands.data());
                            commandQueue->WaitIdle();//stagingBuffer‚ÌŽõ–½‚ªØ‚ê‚é‚Ì‚Å‚±‚±‚Å‘Ò‚Â

                            commandAllocator->Reset();
                            commandList->Reset(commandAllocator.Get(), NULL);
                        }

                        D3D12_RESOURCE_BARRIER	resourceBarrier = {};
                        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        resourceBarrier.Transition.pResource = resource.Get();
                        resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
                        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
                        commandList->ResourceBarrier(1, &resourceBarrier);

                        commandList->Close();
                        std::vector<ID3D12CommandList*> commands{ commandList.Get() };
                        auto& commandQueue = GfxContext::GetInstance()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
                        commandQueue->GetCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commands.size()), commands.data());
                        commandQueue->WaitIdle();//stagingBuffer‚ÌŽõ–½‚ªØ‚ê‚é‚Ì‚Å‚±‚±‚Å‘Ò‚Â

                        commandAllocator->Reset();
                        commandList->Reset(commandAllocator.Get(), NULL);
                    }
                private:
                    ComPtr<ID3D12Resource> resource{};
                };
            }
        }
    }
}