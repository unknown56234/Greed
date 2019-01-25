#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Pipeline
        {
            class Pipeline
            {
            public:
                ComPtr<ID3D12PipelineState> GetPipelineState()
                {
                    return pipelineState;
                }
                ComPtr<ID3D12RootSignature> GetRootSignature()
                {
                    return rootSignature;
                }
                //Builder����Z�b�g����s��
                //TODO �Ȃ�Ƃ�����
                void _SetPipelineState(ComPtr<ID3D12PipelineState> pipelineState)
                {
                    this->pipelineState = pipelineState;
                }
                void _SetRootSignature(ComPtr<ID3D12RootSignature> rootSignature)
                {
                    this->rootSignature = rootSignature;
                }
            private:
                ComPtr<ID3D12PipelineState> pipelineState{};
                ComPtr<ID3D12RootSignature> rootSignature{};
            };
        }
    }
}

#include"Builder/Builder.h"