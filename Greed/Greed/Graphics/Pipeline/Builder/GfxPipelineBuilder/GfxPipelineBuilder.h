#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Pipeline
        {
            namespace Builder
            {
                namespace GfxPipelineBuilder
                {
                    using GfxContext = gr::Graphics::Context::Context;
                    using gr::Graphics::Shader::VertexShader::VertexShader;
                    using gr::Graphics::Shader::PixelShader::PixelShader;
                    using gr::Graphics::Pipeline::Pipeline;
                    class GfxPipelineBuilder
                    {
                    public:
                        using Self = GfxPipelineBuilder;
                        Self& SetVertexShader(ComPtr<ID3DBlob> blob)
                        {
                            vertexShaderBlob = blob;
                            return *this;
                        }
                        Self& SetPixelShader(ComPtr<ID3DBlob> blob)
                        {
                            pixelShaderBlob = blob;
                            return *this;
                        }
                        Self& SetGeometryShader(ComPtr<ID3DBlob> blob)
                        {
                            geometryShaderBlob = blob;
                            return *this;
                        }
                        Self& SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& layouts)
                        {
                            inputLayouts = layouts;
                            return *this;
                        }
                        Self& DepthEnable()
                        {
                            pipelineStateDesc.DepthStencilState.DepthEnable = TRUE;
                            pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
                            pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
                            pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
                            pipelineStateDesc.DepthStencilState.StencilReadMask = 0;
                            pipelineStateDesc.DepthStencilState.StencilWriteMask = 0;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
                            pipelineStateDesc.DepthStencilState.BackFace = pipelineStateDesc.DepthStencilState.FrontFace;
                            return *this;
                        }
                        Self& DepthDisable()
                        {
                            pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
                            pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
                            pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
                            pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
                            pipelineStateDesc.DepthStencilState.StencilReadMask = 0;
                            pipelineStateDesc.DepthStencilState.StencilWriteMask = 0;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
                            pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
                            pipelineStateDesc.DepthStencilState.BackFace = pipelineStateDesc.DepthStencilState.FrontFace;
                            return *this;
                        }
                        Self& Solid()
                        {
                            pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
                            pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
                            pipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
                            pipelineStateDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
                            pipelineStateDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
                            pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
                            pipelineStateDesc.RasterizerState.DepthClipEnable = TRUE;
                            pipelineStateDesc.RasterizerState.MultisampleEnable = FALSE;
                            pipelineStateDesc.RasterizerState.AntialiasedLineEnable = FALSE;
                            pipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
                            pipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
                            return *this;
                        }
                        Self& Wireframe()
                        {
                            pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
                            pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
                            pipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
                            pipelineStateDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
                            pipelineStateDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
                            pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
                            pipelineStateDesc.RasterizerState.DepthClipEnable = TRUE;
                            pipelineStateDesc.RasterizerState.MultisampleEnable = FALSE;
                            pipelineStateDesc.RasterizerState.AntialiasedLineEnable = FALSE;
                            pipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
                            pipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
                            return *this;
                        }
                        Self& SolidCullNone()
                        {
                            pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
                            pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
                            pipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
                            pipelineStateDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
                            pipelineStateDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
                            pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
                            pipelineStateDesc.RasterizerState.DepthClipEnable = TRUE;
                            pipelineStateDesc.RasterizerState.MultisampleEnable = FALSE;
                            pipelineStateDesc.RasterizerState.AntialiasedLineEnable = FALSE;
                            pipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
                            pipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
                            return *this;
                        }
                        Self& BlendNone()
                        {
                            pipelineStateDesc.BlendState.AlphaToCoverageEnable = FALSE;
                            pipelineStateDesc.BlendState.IndependentBlendEnable = FALSE;
                            for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
                            {
                                pipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
                                pipelineStateDesc.BlendState.RenderTarget[i].LogicOpEnable = FALSE;
                                pipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
                                pipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
                                pipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
                                pipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
                                pipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
                                pipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
                                pipelineStateDesc.BlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
                                pipelineStateDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
                            }
                            return *this;
                        }
                        std::shared_ptr<Pipeline> Build()
                        {
                            auto pipeline = std::make_shared<Pipeline>();
                            auto& device = GfxContext::GetInstance()->GetDevice();
                            HRESULT hr{};
                            //root signatureをシェーダーから作成
                            ComPtr<ID3DBlob> rootSignatureBlob{};
                            hr = D3DGetBlobPart(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, rootSignatureBlob.GetAddressOf());
                            assert(SUCCEEDED(hr));
                            ComPtr<ID3D12RootSignature> rootSignature{};
                            hr = device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
                            pipeline->_SetRootSignature(rootSignature);
                            if (FAILED(hr))
                            {
                                return nullptr;
                            }

                            pipelineStateDesc.pRootSignature = pipeline->GetRootSignature().Get();
                            pipelineStateDesc.VS.pShaderBytecode = vertexShaderBlob->GetBufferPointer();
                            pipelineStateDesc.VS.BytecodeLength = vertexShaderBlob->GetBufferSize();
                            pipelineStateDesc.PS.pShaderBytecode = pixelShaderBlob->GetBufferPointer();
                            pipelineStateDesc.PS.BytecodeLength = pixelShaderBlob->GetBufferSize();
                            if (geometryShaderBlob)
                            {
                                pipelineStateDesc.GS.pShaderBytecode = geometryShaderBlob->GetBufferPointer();
                                pipelineStateDesc.GS.BytecodeLength = geometryShaderBlob->GetBufferSize();
                            }

                            pipelineStateDesc.SampleMask = UINT_MAX;

                            pipelineStateDesc.InputLayout.NumElements = static_cast<UINT>(inputLayouts.size());
                            pipelineStateDesc.InputLayout.pInputElementDescs = inputLayouts.data();

                            pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
                            pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

                            pipelineStateDesc.NumRenderTargets = 1;
                            pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
                            pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

                            pipelineStateDesc.SampleDesc.Count = 1;
                            pipelineStateDesc.SampleDesc.Quality = 0;

                            pipelineStateDesc.NodeMask = 0;

                            pipelineStateDesc.CachedPSO.pCachedBlob = NULL;
                            pipelineStateDesc.CachedPSO.CachedBlobSizeInBytes = 0;
                            pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

                            ComPtr<ID3D12PipelineState> pipelineState{};
                            hr = device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
                            pipeline->_SetPipelineState(pipelineState);
                            if (FAILED(hr))
                            {
                                return nullptr;
                            }
                            return pipeline;
                        }

                    private:
                        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
                        ComPtr<ID3DBlob> vertexShaderBlob{};
                        ComPtr<ID3DBlob> pixelShaderBlob{};
                        ComPtr<ID3DBlob> geometryShaderBlob{};
                        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts{};
                    };
                }
            }
        }
    }
}