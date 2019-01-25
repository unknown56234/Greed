#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Shader
        {
            namespace VertexShader
            {
                using gr::Graphics::Shader::Shader;
                class VertexShader:public Shader
                {
                public:
                    VertexShader(LPCWSTR path) :
                        Shader(path, "main", "vs_5_0", {})
                    {
                    }
                    VertexShader(LPCWSTR path, const std::vector<D3D_SHADER_MACRO>& macro) :
                        Shader(path, "main", "vs_5_0", macro)
                    {
                    }
                };
            }
        }
    }
}