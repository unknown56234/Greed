#include"Greed/Common/Common.h"
#include"Greed/Graphics/Graphics.h"

//TODO 場所考える
namespace gr
{
    namespace DataStruct
    {
        namespace SceneInfo
        {
            struct SceneInfo
            {
                DirectX::XMFLOAT4X4 viewMatrix{};
                DirectX::XMFLOAT4X4 projectionMatrix{};
                DirectX::XMFLOAT4 viewPosition{};
                DirectX::XMFLOAT4 lightDirection{};
                DirectX::XMFLOAT4 lightColor{};
            };
        }
    }

    namespace Application
    {
        using gr::Common::Window::Window;
        using gr::Graphics::DebugLayer::DebugLayer;
        using GfxContext = gr::Graphics::Context::Context;
        using gr::Graphics::CommandAllocator::CommandAllocator;
        using gr::Graphics::SwapChain::SwapChain;
        using gr::Graphics::DepthBuffer::DepthBuffer;
        using GuiContext = gr::Graphics::Gui::Context::Context;
        using gr::Graphics::Shader::VertexShader::VertexShader;
        using gr::Graphics::Shader::PixelShader::PixelShader;
        using gr::Graphics::Shader::GeometryShader::GeometryShader;
        using Pipeline = gr::Graphics::Pipeline::Pipeline;
        using gr::Graphics::Pipeline::Builder::GfxPipelineBuilder::GfxPipelineBuilder;

        using gr::Graphics::Resource::UploadBuffer::UploadBuffer;

        using gr::Graphics::Model::glTF::ModelData;
        using gr::Graphics::Model::Model;
        using gr::Graphics::Model::ModelReference;

        using gr::Graphics::Resource::CubeMap::CubeMap;
        using gr::Graphics::Resource::Image::Image;

        class Application
        {
        public:
            void Run()
            {
                //GUIセット用パラメータ定義

                DirectX::XMFLOAT4 backGroundColor{};

                DirectX::XMFLOAT3 cameraPosition{ 0.0f,1.0f,0.0f };
                DirectX::XMFLOAT3 cameraTarget{ 0.0f,0.0f,0.0f };
                DirectX::XMFLOAT3 cameraUp{ 0.0f,1.0f,0.0f };

                std::string pipelineBuildResult{};

                gr::DataStruct::SceneInfo::SceneInfo sceneInfo{};
                sceneInfo.lightColor.x = 1.0f;
                sceneInfo.lightColor.y = 1.0f;
                sceneInfo.lightColor.z = 1.0f;
                sceneInfo.lightColor.w = 1.0f;

                float lightRotation{};
                float lightPitch{};

                float viewRotation = 0.0f;
                float viewPitch = 0.0f;
                float viewDistance = 3.0f;

                bool useVSync = true;
                bool useIbl = true;

                auto& directQueue = GfxContext::GetInstance()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

                //メインループ
                window->Loop([&]()
                {
                    guiContext->NewFrame();

                    //各種GUI
                    {
                        if (ImGui::Begin("debug"))
                        {
                            ImGui::Checkbox("use vsync", &useVSync);
                            ImGui::Checkbox("use IBL", &useIbl);

                            if (ImGui::Button("Reload Shader"))
                            {
                                directQueue->WaitIdle();
                                auto res = CreatePipeline(useIbl, false);
                                if (res)
                                {
                                    pipelineBuildResult = "pipeline build success";
                                }
                                else
                                {
                                    pipelineBuildResult = "pipeline build failed";
                                }
                            }
                            ImGui::Text(pipelineBuildResult.c_str());
                        }
                        ImGui::End();

                        if (ImGui::Begin("framerate"))
                        {
                            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                        }
                        ImGui::End();

                        if (ImGui::Begin("background"))
                        {
                            float c[4]{ backGroundColor.x,backGroundColor.y ,backGroundColor.z ,backGroundColor.w };
                            ImGui::ColorPicker4("color", c);
                            backGroundColor.x = c[0];
                            backGroundColor.y = c[1];
                            backGroundColor.z = c[2];
                            backGroundColor.w = c[3];
                        }
                        ImGui::End();

                        if (ImGui::Begin("light info"))
                        {
                            ImGui::SliderFloat("rotation", &lightRotation, -180.0f, 180.0f);
                            ImGui::SliderFloat("pitch", &lightPitch, -90.0f, 90.0f);

                            DirectX::XMFLOAT3 dir{ 0.0f,0.0f,-1.0f };

                            auto v = DirectX::XMLoadFloat3(&dir);
                            auto q = DirectX::XMQuaternionRotationRollPitchYaw(-lightPitch * DirectX::XM_PI / 180.0f, -lightRotation * DirectX::XM_PI / 180.0f, 0.0f);
                            DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Rotate(v, q));
                            sceneInfo.lightDirection.x = dir.x;
                            sceneInfo.lightDirection.y = dir.y;
                            sceneInfo.lightDirection.z = dir.z;

                            float c[4]{ sceneInfo.lightColor.x,sceneInfo.lightColor.y ,sceneInfo.lightColor.z ,sceneInfo.lightColor.w };
                            ImGui::ColorPicker4("color", c);
                            sceneInfo.lightColor.x = c[0];
                            sceneInfo.lightColor.y = c[1];
                            sceneInfo.lightColor.z = c[2];
                            sceneInfo.lightColor.w = c[3];
                        }
                        ImGui::End();

                        if (ImGui::Begin("camera info"))
                        {
                            ImGui::SliderFloat("rotation", &viewRotation, -180.0f, 180.0f);
                            ImGui::SliderFloat("pitch", &viewPitch, -90.0f, 90.0f);
                            ImGui::DragFloat("distance", &viewDistance, 0.01f, 0.1f, 10.0f);

                            DirectX::XMFLOAT3 dir{ 0.0f,0.0f,-1.0f };
                            DirectX::XMFLOAT3 up{ 0.0f,1.0f,0.0f };
                            auto v = DirectX::XMLoadFloat3(&dir);
                            auto u = DirectX::XMLoadFloat3(&up);
                            auto q = DirectX::XMQuaternionRotationRollPitchYaw(-viewPitch * DirectX::XM_PI / 180.0f, -viewRotation * DirectX::XM_PI / 180.0f, 0.0f);
                            v = DirectX::XMVector3Normalize(v);
                            DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Rotate(v, q));
                            cameraPosition.x = -dir.x*viewDistance;
                            cameraPosition.y = -dir.y*viewDistance;
                            cameraPosition.z = -dir.z*viewDistance;

                            DirectX::XMStoreFloat3(&cameraUp, DirectX::XMVector3Rotate(u, q));
                        }
                        ImGui::End();
                    }

                    //シェーダーに送るシーン情報更新
                    {
                        auto ptr = viewProjection->Map();
                        {
                            auto position = DirectX::XMLoadFloat3(&cameraPosition);
                            auto target = DirectX::XMLoadFloat3(&cameraTarget);
                            auto up = DirectX::XMLoadFloat3(&cameraUp);
                            auto viewMat = DirectX::XMMatrixLookAtLH(position, target, up);
                            auto projectionMat = DirectX::XMMatrixPerspectiveFovLH(60.0f / 180.0f*DirectX::XM_PI, static_cast<float>(swapchain->GetWidth()) / static_cast<float>(swapchain->GetHeight()), 0.1f, 512.0f);

                            DirectX::XMStoreFloat4x4(&sceneInfo.viewMatrix, viewMat);
                            DirectX::XMStoreFloat4x4(&sceneInfo.projectionMatrix, projectionMat);
                            sceneInfo.viewPosition.x = cameraPosition.x;
                            sceneInfo.viewPosition.y = cameraPosition.y;
                            sceneInfo.viewPosition.z = cameraPosition.z;
                        }
                        memcpy(ptr, &sceneInfo, sizeof(sceneInfo));
                        viewProjection->Unmap();
                    }
                    modelReference->Update();

                    //描画

                    D3D12_VIEWPORT viewport{};
                    D3D12_RECT scissorRect{};
                    D3D12_CPU_DESCRIPTOR_HANDLE	dsvHandle = depthBuffer->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
                    swapchain->BeginRender(directCommandList, backGroundColor.x, backGroundColor.y, backGroundColor.z, backGroundColor.w, &dsvHandle);
                    D3D12_RECT rc{};
                    rc.left = 0;
                    rc.top = 0;
                    rc.right = swapchain->GetWidth();
                    rc.bottom = swapchain->GetHeight();
                    directCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &rc);

                    {
                        PIXBeginEvent(directCommandList.Get(), 0, "draw model");
                        directCommandList->SetPipelineState(gfxPipeline->GetPipelineState().Get());
                        directCommandList->SetGraphicsRootSignature(gfxPipeline->GetRootSignature().Get());
                        viewport.TopLeftX = 0.0f;
                        viewport.TopLeftY = 0.0f;
                        viewport.Width = static_cast<float>(swapchain->GetWidth());
                        viewport.Height = static_cast<float>(swapchain->GetHeight());
                        viewport.MinDepth = 0.0f;
                        viewport.MaxDepth = 1.0f;

                        scissorRect.left = 0;
                        scissorRect.top = 0;
                        scissorRect.right = static_cast<LONG>(swapchain->GetWidth());
                        scissorRect.bottom = static_cast<LONG>(swapchain->GetHeight());

                        directCommandList->RSSetViewports(1, &viewport);
                        directCommandList->RSSetScissorRects(1, &scissorRect);

                        directCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                        directCommandList->SetGraphicsRootConstantBufferView(0, viewProjection->GetResource()->GetGPUVirtualAddress());
                        directCommandList->SetGraphicsRootConstantBufferView(1, modelReference->GetNodeBuffer()->GetResource()->GetGPUVirtualAddress());

                        //test
                        //directCommandList->SetGraphicsRootShaderResourceView(4, );


                        int nodeCount = model->GetNodes().size();
                        for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
                        {
                            const auto& node = model->GetNodes()[nodeIndex];
                            if (auto meshIndex = node->GetMesh(); meshIndex != -1)
                            {
                                directCommandList->SetGraphicsRoot32BitConstant(3, nodeIndex, 0);
                                const auto& mesh = model->GetMeshes()[meshIndex];
                                int primitiveCount = mesh.size();
                                for (int primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++)
                                {
                                    const auto& primitive = mesh[primitiveIndex];
                                    auto materialIndex = model->GetMaterialIndices()[meshIndex][primitiveIndex];
                                    std::array<ID3D12DescriptorHeap*, 1> heaps = { model->GetMaterials()[materialIndex]->GetDescriptorHeap().Get() };
                                    directCommandList->SetDescriptorHeaps(1, heaps.data());
                                    directCommandList->SetGraphicsRootDescriptorTable(2, heaps[0]->GetGPUDescriptorHandleForHeapStart());
                                    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews{
                                        primitive->GetVertexBuffer("POSITION")->GetBufferView(),
                                        primitive->GetVertexBuffer("NORMAL")->GetBufferView(),
                                        primitive->GetVertexBuffer("TEXCOORD")->GetBufferView(),
                                        primitive->GetVertexBuffer("COLOR")->GetBufferView(),
                                        primitive->GetVertexBuffer("TANGENT")->GetBufferView(),
                                    };
                                    auto indexBufferView = primitive->GetIndexBuffer()->GetBufferView();

                                    directCommandList->IASetVertexBuffers(0, vertexBufferViews.size(), vertexBufferViews.data());
                                    directCommandList->IASetIndexBuffer(&indexBufferView);
                                    directCommandList->DrawIndexedInstanced(primitive->GetIndexCount(), 1, 0, 0, 0);
                                }
                            }
                        }
                        PIXEndEvent(directCommandList.Get());
                    }


                    PIXBeginEvent(directCommandList.Get(), 0, "draw gui");
                    guiContext->Render(directCommandList);
                    PIXEndEvent(directCommandList.Get());

                    swapchain->EndRender(directCommandList);

                    directCommandList->Close();

                    //描画終わり

                    //コマンド送信、待機、commandList commandAllocatorのリセット
                    std::vector<ID3D12CommandList*> commands{ directCommandList.Get() };
                    UINT commandBufferCount = static_cast<UINT>(commands.size());
                    directQueue->GetCommandQueue()->ExecuteCommandLists(commandBufferCount, commands.data());

                    auto hr = swapchain->GetSwapchain()->Present(useVSync ? 1 : 0, 0);
                    assert(SUCCEEDED(hr));

                    directQueue->WaitIdle();

                    directCommandAllocator->Reset();
                    directCommandList->Reset(directCommandAllocator->GetCommandAllocator().Get(), NULL);
                });
            }
            Application(HINSTANCE hInstance)
            {
#ifdef _DEBUG
                gr::Graphics::DebugLayer::DebugLayer::Enable();
#endif
                window = std::make_shared<Window>(hInstance, 1280, 720);

                GfxContext::CreateInstance();

                directCommandAllocator = GfxContext::GetInstance()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
                directCommandAllocator->Allocate(0, nullptr, IID_PPV_ARGS(directCommandList.GetAddressOf()));

                swapchain = std::make_shared<SwapChain>(window->GetWindowHandle(), 3);
                depthBuffer = std::make_shared<DepthBuffer>(window->GetWidth(), window->GetHeight());

                guiContext = std::make_shared<GuiContext>(window->GetWindowHandle(), 3);

                //ウィンドウプロシージャにイベント登録
                window->SetWindowEvent([&](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
                {
                    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
                    {
                        return S_OK;
                    }
                    switch (message)
                    {
                    case WM_KEYDOWN: {
                        if (wParam == VK_ESCAPE)
                        {
                            PostQuitMessage(0);
                        }
                        break;
                    }
                    case WM_SIZE: {
                        //swapchain depthbuffer ImGui周りの再生成
                        if (GfxContext::GetInstance() != nullptr&&swapchain != nullptr&&wParam != SIZE_MINIMIZED)
                        {
                            ImGui_ImplDX12_InvalidateDeviceObjects();
                            auto& directQueue = GfxContext::GetInstance()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
                            directQueue->WaitIdle();
                            swapchain->Resize(directQueue->GetCommandQueue(), hWnd, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
                            ImGui_ImplDX12_CreateDeviceObjects();
                            depthBuffer = std::make_shared<DepthBuffer>((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    return S_OK;
                });
                CreateResources();
                window->Show();
            }
            ~Application()
            {
                GfxContext::DestroyInstance();
            }
        private:
            void CreateResources()
            {
                CreatePipeline(true);

                viewProjection = std::make_shared<UploadBuffer>(sizeof(DirectX::XMFLOAT4X4) * 2);

                //モデルのロード
                {
                    tinygltf::Model modelData{};
                    tinygltf::TinyGLTF loader{};
                    std::string err{};
                    std::string warn{};
                    //loader.LoadASCIIFromFile(&modelData, &err, &warn, "C:\\Users\\2160065\\Documents\\workspace\\glTF-Sample-Models\\2.0\\MetalRoughSpheres\\glTF\\MetalRoughSpheres.gltf");
                    loader.LoadASCIIFromFile(&modelData, &err, &warn, "C:\\Users\\2160065\\Documents\\workspace\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf");
                    //loader.LoadASCIIFromFile(&modelData, &err, &warn, "C:\\Users\\2160065\\Documents\\workspace\\glTF-Sample-Models\\2.0\\Buggy\\glTF\\Buggy.gltf");
                    //loader.LoadASCIIFromFile(&modelData, &err, &warn, "C:\\Users\\2160065\\Documents\\workspace\\glTF-Sample-Models\\2.0\\FlightHelmet\\glTF\\FlightHelmet.gltf");
                    //loader.LoadASCIIFromFile(&modelData, &err, &warn, "C:\\Users\\2160065\\Documents\\workspace\\glTF-Sample-Models\\2.0\\BoomBox\\glTF\\BoomBox.gltf");

                    model = std::make_shared<gr::Graphics::Model::Model>(
                        directCommandAllocator->GetCommandAllocator(),
                        directCommandList,
                        modelData
                        );
                    modelReference = std::make_shared<ModelReference>(model);
                    modelReference->Update();
                }

                {
                    auto loader = std::make_unique<gr::Common::Resource::ImageLoader::ImageLoader>();
                    auto createImageResourceFromScratchImage = [&](const DirectX::ScratchImage& scratchImage)
                    {
                        ComPtr<ID3D12Resource> image{};
                        DirectX::CreateTextureEx(GfxContext::GetInstance()->GetDevice().Get(), scratchImage.GetMetadata(), D3D12_RESOURCE_FLAG_NONE, false, image.GetAddressOf());
                        std::vector<D3D12_SUBRESOURCE_DATA> subresources{};
                        DirectX::PrepareUpload(GfxContext::GetInstance()->GetDevice().Get(), scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), subresources);
                        auto uploadBufferSize = GetRequiredIntermediateSize(image.Get(), 0, subresources.size());

                        auto stagingBuffer = std::make_unique<UploadBuffer>(uploadBufferSize);

                        UpdateSubresources(directCommandList.Get(),
                            image.Get(), stagingBuffer->GetResource().Get(),
                            0, 0, static_cast<unsigned int>(subresources.size()),
                            subresources.data());

                        D3D12_RESOURCE_BARRIER	resourceBarrier = {};
                        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        resourceBarrier.Transition.pResource = image.Get();
                        resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
                        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
                        directCommandList->ResourceBarrier(1, &resourceBarrier);

                        directCommandList->Close();
                        std::vector<ID3D12CommandList*> commands{ directCommandList.Get() };
                        auto& commandQueue = GfxContext::GetInstance()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
                        commandQueue->GetCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commands.size()), commands.data());
                        commandQueue->WaitIdle();//stagingBufferの寿命が切れるのでここで待つ

                        directCommandAllocator->Reset();
                        directCommandList->Reset(directCommandAllocator->GetCommandAllocator().Get(), NULL);
                        return image;
                    };

                    DirectX::ScratchImage scratchImage{};
                    scratchImage = loader->Load("Resource\\Texture\\cubemap\\yokohama_dds\\yokohamaDiffuseHDR.dds");
                    cubeMapDiffuse = std::make_unique<Image>(directCommandAllocator->GetCommandAllocator(),directCommandList,scratchImage);

                    scratchImage = loader->Load("Resource\\Texture\\cubemap\\yokohama_dds\\yokohamaSpecularHDR.dds");
                    cubeMapSpecular = std::make_unique<Image>(directCommandAllocator->GetCommandAllocator(), directCommandList, scratchImage);

                    scratchImage = loader->Load("Resource\\Texture\\cubemap\\yokohama_dds\\yokohamaBrdf.dds");
                    brdfImage = std::make_unique<Image>(directCommandAllocator->GetCommandAllocator(), directCommandList, scratchImage);

                    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
                    descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                    descriptorHeapDesc.NumDescriptors = 3;
                    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                    descriptorHeapDesc.NodeMask = 0;
                    GfxContext::GetInstance()->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(cubeMapDescriptorHeap.GetAddressOf()));
                    
                    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                    srvDesc.Format = cubeMapDiffuse->GetFormat();
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                    srvDesc.TextureCube.MipLevels = cubeMapDiffuse->GetResource()->GetDesc().MipLevels;
                    srvDesc.TextureCube.MostDetailedMip = 0;
                    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                    auto handle = cubeMapDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                    GfxContext::GetInstance()->GetDevice()->CreateShaderResourceView(cubeMapDiffuse->GetResource().Get(), &srvDesc, handle);

                    srvDesc.Format = cubeMapSpecular->GetFormat();
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                    srvDesc.TextureCube.MipLevels = cubeMapSpecular->GetResource()->GetDesc().MipLevels;
                    srvDesc.TextureCube.MostDetailedMip = 0;
                    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                    handle.ptr += GfxContext::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    GfxContext::GetInstance()->GetDevice()->CreateShaderResourceView(cubeMapSpecular->GetResource().Get(), &srvDesc, handle);

                    srvDesc.Format = brdfImage->GetFormat();
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                    srvDesc.TextureCube.MipLevels = brdfImage->GetResource()->GetDesc().MipLevels;
                    srvDesc.TextureCube.MostDetailedMip = 0;
                    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                    handle.ptr += GfxContext::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    GfxContext::GetInstance()->GetDevice()->CreateShaderResourceView(brdfImage->GetResource().Get(), &srvDesc, handle);
                    for (auto&& material : model->GetMaterials())
                    {
                        material->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
                        handle = material->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
                        handle.ptr += GfxContext::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;
                        GfxContext::GetInstance()->GetDevice()->CopyDescriptorsSimple(3, handle, cubeMapDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    }
                }
            }
            //forceがfalseのときは作成失敗時に現状維持する
            bool CreatePipeline(bool useIbl, bool force = true)
            {
                std::vector<D3D_SHADER_MACRO> macros = {};
                if (useIbl)
                {
                    macros.push_back({ "USE_IBL", "" });
                }
                macros.push_back({ NULL, NULL });
                auto vertexShader = std::make_unique<VertexShader>(L"Resource\\Shader\\Test\\vertex.hlsl");
                auto pixelShader = std::make_unique<PixelShader>(L"Resource\\Shader\\Test\\pixel.hlsl", macros);

                if (FAILED(vertexShader->GetCompileResult()) || FAILED(pixelShader->GetCompileResult()))
                {
                    if (force)
                    {
                        panic("failed to compile shader");
                    }
                    return false;
                }


                std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts{
                    {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
                    {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,1,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
                    {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,2,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
                    {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,3,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
                    {"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,4,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
                };
                auto pipeline = GfxPipelineBuilder()
                    .SetVertexShader(vertexShader->GetBlob())
                    .SetPixelShader(pixelShader->GetBlob())
                    .SetInputLayout(inputLayouts)
                    .DepthEnable()
                    .Solid()
                    .BlendNone()
                    .Build();
                if (pipeline)
                {
                    gfxPipeline = pipeline;
                    return true;
                }
                else if (force)
                {
                    panic("failed to build pipeline");
                    return false;
                }
                else
                {
                    return false;
                }
            }
            std::shared_ptr<Window> window{};
            std::shared_ptr<CommandAllocator> directCommandAllocator{};
            ComPtr<ID3D12GraphicsCommandList> directCommandList{};

            std::shared_ptr<SwapChain> swapchain{};
            std::shared_ptr<DepthBuffer> depthBuffer{};

            std::shared_ptr<GuiContext> guiContext{};

            std::shared_ptr<Pipeline> gfxPipeline{};

            std::shared_ptr<UploadBuffer> viewProjection{};

            std::shared_ptr<Model> model{};
            std::shared_ptr<ModelReference> modelReference{};

            std::unique_ptr<Image> cubeMapDiffuse{};
            std::unique_ptr<Image> cubeMapSpecular{};
            std::unique_ptr<Image> brdfImage{};
            ComPtr<ID3D12DescriptorHeap> cubeMapDescriptorHeap{};
        };
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmd)
{
#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize))
    {
        panic("failed to com initialize");
    }
#else
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
    {
        panic("failed to com initialize");
    }

#endif
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmd;

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    auto app = std::make_unique<gr::Application::Application>(hInstance);
    app->Run();
}