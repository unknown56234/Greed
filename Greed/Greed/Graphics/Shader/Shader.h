#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Shader
        {
            class Shader
            {
            public:
                ComPtr<ID3DBlob> GetBlob() const
                {
                    return blob;
                }
                HRESULT GetCompileResult() const
                {
                    return compileResult;
                }
                Shader(LPCWSTR path,LPCSTR entryPoint,LPCSTR target, const std::vector<D3D_SHADER_MACRO>& macro)
                {
                    
#ifdef _DEBUG
                    UINT compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else 
                    UINT compileFlag = 0;
#endif
                    ComPtr<ID3DBlob> errorBlob{};
                    compileResult = D3DCompileFromFile(path, macro.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, compileFlag, 0, blob.GetAddressOf(), errorBlob.GetAddressOf());
                }
            private:
                ComPtr<ID3DBlob> blob{};
                HRESULT compileResult{};

            };
        }
    }
}
#include"VertexShader/VertexShader.h"
#include"PixelShader/PixelShader.h"
#include"GeometryShader/GeometryShader.h"