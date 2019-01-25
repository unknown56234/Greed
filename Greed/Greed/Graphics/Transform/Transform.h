#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Transform
        {
            using namespace DirectX;

            class Transform
            {
            public:
                Transform() :
                    translation({ 0.0f,0.0f,0.0f }),
                    rotation({ 0.0f,0.0f,0.0f,1.0f }),
                    scale({ 1.0f,1.0f,1.0f })
                {
                };
                Transform(const XMFLOAT4X4& mat) :
                    translation({ 0.0f,0.0f,0.0f }),
                    rotation({ 0.0f,0.0f,0.0f,1.0f }),
                    scale({ 1.0f,1.0f,1.0f })
                {
                    SetFromMatrix(mat);
                }
                Transform(const XMFLOAT3& translation, const XMFLOAT4& rotation, const XMFLOAT3& scale) :
                    translation(translation),
                    rotation(rotation),
                    scale(scale)
                {
                }

                XMFLOAT3 GetTranslation() const { return translation; }
                void SetTranslation(const XMFLOAT3& translation) { this->translation = translation; }
                void SetTranslation(float x, float y, float z)
                {
                    translation.x = x;
                    translation.y = y;
                    translation.z = z;
                }

                XMFLOAT4 GetRotation() const { return rotation; }
                void SetRotation(const XMFLOAT4& rotation) { this->rotation = rotation; }
                void SetRotation(float x, float y, float z, float w)
                {
                    rotation.x = x;
                    rotation.y = y;
                    rotation.z = z;
                    rotation.w = w;
                }

                XMFLOAT3 GetScale() const { return scale; }
                void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
                void SetScale(float x, float y, float z)
                {
                    scale.x = x;
                    scale.y = y;
                    scale.z = z;
                }

                void SetFromMatrix(const XMFLOAT4X4& mat)
                {

                    translation = XMFLOAT3(mat._41, mat._42, mat._43);
                    auto x = XMFLOAT3(mat._11, mat._12, mat._13);
                    auto y = XMFLOAT3(mat._21, mat._22, mat._23);
                    auto z = XMFLOAT3(mat._31, mat._32, mat._33);
                    auto vecX = XMLoadFloat3(&x);
                    auto vecY = XMLoadFloat3(&y);
                    auto vecZ = XMLoadFloat3(&z);
                    XMStoreFloat(&scale.x, XMVector3Dot(vecX, vecX));
                    XMStoreFloat(&scale.y, XMVector3Dot(vecY, vecY));
                    XMStoreFloat(&scale.z, XMVector3Dot(vecZ, vecZ));
                    XMStoreFloat4(
                        &rotation,
                        XMQuaternionNormalize(
                            XMQuaternionRotationMatrix(
                                XMMatrixMultiply(
                                    XMMatrixScaling(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z),
                                    XMLoadFloat4x4(&mat)
                                )
                            )
                        )
                    );
                }
                XMFLOAT4X4 CalclateMatrix() const
                {
                    auto T = XMMatrixTranslation(translation.x, translation.y, translation.z);
                    auto R = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
                    auto S = XMMatrixScaling(scale.x, scale.y, scale.z);
                    XMFLOAT4X4 res{};
                    XMStoreFloat4x4(&res, S*R*T);
                    return res;
                }
            private:
                XMFLOAT3 translation{};
                XMFLOAT4 rotation{};
                XMFLOAT3 scale{};
            };
        }
    }
}