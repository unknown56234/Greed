#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Shader
        {
            namespace PixelShader
            {
                using gr::Graphics::Shader::Shader;
                class PixelShader :public Shader
                {
                public:
                    PixelShader(LPCWSTR path) :
                        Shader(path, "main", "ps_5_0", {})
                    {

                    }
                    PixelShader(LPCWSTR path, const std::vector<D3D_SHADER_MACRO>& macro) :
                        Shader(path, "main", "ps_5_0", macro)
                    {

                    }
                };
            }
        }
    }
}