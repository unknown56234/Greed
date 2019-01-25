#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Shader
        {
            namespace GeometryShader
            {
                using gr::Graphics::Shader::Shader;
                class GeometryShader :public Shader
                {
                public:
                    GeometryShader(LPCWSTR path) :
                        Shader(path, "main", "gs_5_0", {})
                    {

                    }
                    GeometryShader(LPCWSTR path, const std::vector<D3D_SHADER_MACRO>& macro) :
                        Shader(path, "main", "gs_5_0", macro)
                    {

                    }

                };
            }
        }
    }
}