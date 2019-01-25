#pragma once
namespace gr
{
    namespace Graphics
    {
        namespace Model
        {
            namespace glTF
            {
                using gr::Graphics::Transform::Transform;

                //tinygltfのバイナリ部分をパースするクラス
                //バイナリ部の数値情報はモノによって型が違うのでここで吸収して一律に扱えるようにする
                class BinalyPerser
                {
                public:
                    union Value
                    {
                        int intData;
                        unsigned int uintData;
                        float floatData;
                        Value() :intData(0) {}
                        Value(int val) :intData(val) {};
                        Value(unsigned int val) :uintData(val) {};
                        Value(float val) :floatData(val) {};
                    };
                    struct Values
                    {
                        enum class Type
                        {
                            eInt,
                            eUint,
                            eFloat
                        };
                        Type elementType{};
                        int valueType{};
                        std::vector<Value> elements{};
                    };

                    void GetData(int accessorIndex, Values& values) const
                    {
                        auto accessor = model.accessors[accessorIndex];

                        values.valueType = accessor.type;
                        switch (accessor.componentType)
                        {
                        case TINYGLTF_COMPONENT_TYPE_BYTE:
                        {
                            GetValueFromBuffer<char>(accessor, values.elements);
                            values.elementType = Values::Type::eInt;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        {
                            GetValueFromBuffer<unsigned char>(accessor, values.elements);
                            values.elementType = Values::Type::eUint;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_SHORT:
                        {
                            GetValueFromBuffer<short>(accessor, values.elements);
                            values.elementType = Values::Type::eInt;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        {
                            GetValueFromBuffer<unsigned short>(accessor, values.elements);
                            values.elementType = Values::Type::eUint;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_INT:
                        {
                            GetValueFromBuffer<int>(accessor, values.elements);
                            values.elementType = Values::Type::eInt;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        {
                            GetValueFromBuffer<unsigned int>(accessor, values.elements);
                            values.elementType = Values::Type::eUint;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        {
                            GetValueFromBuffer<float>(accessor, values.elements);
                            values.elementType = Values::Type::eFloat;
                        }break;
                        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                        {
                            GetValueFromBuffer<double>(accessor, values.elements);
                            values.elementType = Values::Type::eFloat;
                        }break;
                        default:
                            break;
                        }
                    }

                    BinalyPerser(const tinygltf::Model& model) :
                        model(model)
                    {
                    }

                private:
                    /*
                    bufferからデータをT型で読み込みValue型にして返す
                    */
                    template<class T>
                    void GetValueFromBuffer(const tinygltf::Accessor& accessor, std::vector<Value>& elements) const
                    {
                        size_t size{};
                        std::vector<T> data{};
                        GetBufferData(accessor, size, nullptr);
                        data.resize(size / sizeof(T));
                        GetBufferData(accessor, size, data.data());

                        switch (accessor.componentType)
                        {
                        case TINYGLTF_COMPONENT_TYPE_BYTE:
                        case TINYGLTF_COMPONENT_TYPE_SHORT:
                        case TINYGLTF_COMPONENT_TYPE_INT:
                        {
                            for (auto elem : data)
                                elements.emplace_back(static_cast<int>(elem));
                        }
                        break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        {
                            for (auto elem : data)
                                elements.emplace_back(static_cast<unsigned int>(elem));
                        }
                        break;
                        case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                        {
                            for (auto elem : data)
                                elements.emplace_back(static_cast<float>(elem));
                        }
                        break;
                        default:
                            break;
                        }
                    }

                    /*
                    bufferからデータを読み込む
                    */
                    void GetBufferData(const tinygltf::Accessor& accessor, size_t& size, void* data) const
                    {
                        const auto& bufferView = model.bufferViews[accessor.bufferView];

                        const uint8_t* bufferPtr = model.buffers.at(bufferView.buffer).data.data();
                        bufferPtr += bufferView.byteOffset;

                        size = accessor.count * GetComponentSize(accessor.componentType) * GetAccessorTypeValueCount(accessor.type);

                        if (data != nullptr)
                        {
                            bufferPtr += accessor.byteOffset;
                            memcpy(data, bufferPtr, size);
                        }
                    }
                    static size_t GetComponentSize(int type)
                    {
                        switch (type)
                        {
                        case TINYGLTF_COMPONENT_TYPE_BYTE:
                            return 1;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            return 1;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_SHORT:
                            return 2;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            return 2;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_INT:
                            return 4;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                            return 4;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_FLOAT:
                            return 4;
                            break;
                        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                            return 8;
                            break;
                        default:
                            return 0;
                            break;
                        }
                    }
                    static size_t GetAccessorTypeValueCount(int type)
                    {
                        switch (type)
                        {
                        case TINYGLTF_TYPE_SCALAR:
                            return 1;
                            break;
                        case TINYGLTF_TYPE_VEC2:
                            return 2;
                            break;
                        case TINYGLTF_TYPE_VEC3:
                            return 3;
                            break;
                        case TINYGLTF_TYPE_VEC4:
                            return 4;
                            break;
                        case TINYGLTF_TYPE_MAT2:
                            return 4;
                            break;
                        case TINYGLTF_TYPE_MAT3:
                            return 9;
                            break;
                        case TINYGLTF_TYPE_MAT4:
                            return 16;
                            break;
                        default:
                            return 0;
                            break;
                        }
                    }

                    const tinygltf::Model& model;
                };
                /*
                glTFのScene情報を持つ構造体
                */
                struct Scene
                {
                    Scene(const tinygltf::Scene& scene)
                    {
                        nodes = scene.nodes;
                    }
                    std::vector<int> nodes{};
                };

                /*
                glTFのNode情報を持つ構造体
                */
                struct Node
                {
                    Node(const tinygltf::Node& node)
                    {
                        name = node.name;
                        mesh = node.mesh;
                        skin = node.skin;
                        camera = node.camera;
                        children = node.children;

                        if (node.matrix.size() != 0)
                        {
                            transform.SetFromMatrix(
                                DirectX::XMFLOAT4X4(
                                    static_cast<float>(node.matrix[0]),
                                    static_cast<float>(node.matrix[1]),
                                    static_cast<float>(node.matrix[2]),
                                    static_cast<float>(node.matrix[3]),
                                    static_cast<float>(node.matrix[4]),
                                    static_cast<float>(node.matrix[5]),
                                    static_cast<float>(node.matrix[6]),
                                    static_cast<float>(node.matrix[7]),
                                    static_cast<float>(node.matrix[8]),
                                    static_cast<float>(node.matrix[9]),
                                    static_cast<float>(node.matrix[10]),
                                    static_cast<float>(node.matrix[11]),
                                    static_cast<float>(node.matrix[12]),
                                    static_cast<float>(node.matrix[13]),
                                    static_cast<float>(node.matrix[14]),
                                    static_cast<float>(node.matrix[15])
                                )
                            );
                        }
                        else
                        {
                            if (node.translation.size() != 0)
                                transform.SetTranslation(
                                    static_cast<float>(node.translation[0]),
                                    static_cast<float>(node.translation[1]),
                                    static_cast<float>(node.translation[2])
                                );
                            if (node.rotation.size() != 0)
                                transform.SetRotation(
                                    static_cast<float>(node.rotation[0]),
                                    static_cast<float>(node.rotation[1]),
                                    static_cast<float>(node.rotation[2]),
                                    static_cast<float>(node.rotation[3])
                                );
                            if (node.scale.size() != 0)
                                transform.SetScale(
                                    static_cast<float>(node.scale[0]),
                                    static_cast<float>(node.scale[1]),
                                    static_cast<float>(node.scale[2])
                                );
                        }
                    }

                    std::string name{};
                    int mesh{};
                    int skin{};
                    int camera{};
                    std::vector<int> children{};
                    Transform transform{};
                };

                /*
                glTFのPrimitive情報を持つ構造体
                */
                struct Primitive
                {
                    Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model)
                    {
                        auto helper = BinalyPerser(model);
                        helper.GetData(primitive.indices, indices);

                        auto GetVertexElement = [&](const std::string& attribute, bool& exists, BinalyPerser::Values& values)
                        {
                            exists = primitive.attributes.find(attribute) != primitive.attributes.end();
                            if (exists)
                                helper.GetData(primitive.attributes.at(attribute), values);
                        };

                        GetVertexElement("POSITION", hasPositions, positions);
                        GetVertexElement("NORMAL", hasNormals, normals);
                        GetVertexElement("TANGENT", hasTangents, tangents);
                        GetVertexElement("COLOR", hasColors, colors);
                        GetVertexElement("TEXCOORD_0", hasTexcoords, texcoords);
                        GetVertexElement("JOINTS_0", hasJoints, joints);
                        GetVertexElement("WEIGHTS_0", hasWeights, weights);

                        material = primitive.material;
                    }

                    BinalyPerser::Values indices{};

                    bool hasPositions{};
                    bool hasNormals{};
                    bool hasTangents{};
                    bool hasColors{};
                    bool hasTexcoords{};
                    bool hasJoints{};
                    bool hasWeights{};

                    BinalyPerser::Values positions{};
                    BinalyPerser::Values normals{};
                    BinalyPerser::Values tangents{};
                    BinalyPerser::Values colors{};
                    BinalyPerser::Values texcoords{};
                    BinalyPerser::Values joints{};
                    BinalyPerser::Values weights{};

                    int material;
                };

                /*
                glTFのMesh情報を持つ構造体
                */
                struct Mesh
                {
                    Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model)
                    {
                        for (const auto& primitive : mesh.primitives)
                            primitives.emplace_back(primitive, model);

                    }
                    std::vector<Primitive> primitives{};
                };

                /*
                glTFのSkin情報を持つ構造体
                */
                struct Skin
                {
                    Skin(const tinygltf::Skin& skin, const tinygltf::Model& model)
                    {
                        BinalyPerser helper(model);
                        joints = skin.joints;
                        jointCount = static_cast<int>(joints.size());
                        inverseBindMatrices.resize(jointCount);
                        BinalyPerser::Values matrices{};
                        helper.GetData(skin.inverseBindMatrices, matrices);
                        memcpy(inverseBindMatrices.data(), matrices.elements.data(), sizeof(float)*matrices.elements.size());
                    }
                    std::vector<int> joints{};
                    std::vector<DirectX::XMFLOAT4X4> inverseBindMatrices{};

                    int jointCount{};
                };

                /*
                glTFのAnimation情報の一部を持つヘルパー構造体
                timesとvaluesのindexが対応し、特定時間での情報を取り出すことができる
                */
                template<typename T>
                struct TimeValues
                {
                    TimeValues(void* times, void* values, int elementCount)
                    {
                        this->times.resize(elementCount);
                        this->values.resize(elementCount);
                        memcpy(this->times.data(), times, sizeof(float)*elementCount);
                        memcpy(this->values.data(), values, sizeof(T)*elementCount);
                    }

                    std::vector<float> times{};
                    std::vector<T> values{};
                };

                /*
                glTFのAnimation情報の一部を持つヘルパー構造体
                一つのNodeに対してのAnimation情報を持つ
                */
                struct NodeAnimation
                {
                    void SetTranslationData(void* times, void* translations, int elementCount)
                    {
                        this->translations = std::make_shared<TimeValues<DirectX::XMFLOAT3>>(times, translations, elementCount);
                    }
                    void SetRotationData(void* times, void* rotations, int elementCount)
                    {
                        this->rotations = std::make_shared<TimeValues<DirectX::XMFLOAT4>>(times, rotations, elementCount);
                    }
                    void SetScaleData(void* times, void* scales, int elementCount)
                    {
                        this->scales = std::make_shared<TimeValues<DirectX::XMFLOAT3>>(times, scales, elementCount);
                    }

                    std::shared_ptr<TimeValues<DirectX::XMFLOAT3>> translations{};
                    std::shared_ptr<TimeValues<DirectX::XMFLOAT4>> rotations{};
                    std::shared_ptr<TimeValues<DirectX::XMFLOAT3>> scales{};
                };

                /*
                glTFのAnimation情報を持つ構造体
                */
                struct Animation
                {
                    Animation(const tinygltf::Animation& animation, const tinygltf::Model& model)
                    {
                        BinalyPerser helper(model);
                        for (const auto& channel : animation.channels)
                        {
                            const auto& sampler = animation.samplers[channel.sampler];

                            BinalyPerser::Values input{};
                            BinalyPerser::Values output{};
                            helper.GetData(sampler.input, input);
                            helper.GetData(sampler.output, output);
                            if (channel.target_path == "translation")
                            {
                                nodeTransforms[channel.target_node].SetTranslationData(
                                    input.elements.data(),
                                    output.elements.data(),
                                    static_cast<int>(input.elements.size())
                                );
                            }
                            else if (channel.target_path == "rotation")
                            {
                                nodeTransforms[channel.target_node].SetRotationData(
                                    input.elements.data(),
                                    output.elements.data(),
                                    static_cast<int>(input.elements.size())
                                );
                            }
                            else if (channel.target_path == "scale")
                            {
                                nodeTransforms[channel.target_node].SetScaleData(
                                    input.elements.data(),
                                    output.elements.data(),
                                    static_cast<int>(input.elements.size())
                                );
                            }
                        }
                    }

                    std::map<int, NodeAnimation> nodeTransforms{};
                };

                /*
                glTFのMaterial情報を持つ構造体
                */
                struct Material
                {
                    Material(const tinygltf::Material& material)
                    {
                        baseColorTexture = -1;
                        baseColorFactor.x = 1.0f;
                        baseColorFactor.y = 1.0f;
                        baseColorFactor.z = 1.0f;
                        baseColorFactor.w = 1.0f;
                        metallicRoughnessTexture = -1;
                        metallicFactor = 0.0f;
                        roughnessFactor = 0.0f;
                        normalTexture = -1;

                        auto ExistFunc = [&](const std::string& key, std::function<void(const tinygltf::Parameter&)> func)
                        {
                            if (material.values.find(key) != material.values.end())
                                func(material.values.at(key));
                            else if (material.additionalValues.find(key) != material.additionalValues.end())
                                func(material.additionalValues.at(key));
                        };
                        ExistFunc("baseColorTexture", [&](const tinygltf::Parameter& param)
                        {
                            baseColorTexture = static_cast<int>(param.json_double_value.at("index"));
                        });
                        ExistFunc("baseColorFactor", [&](const tinygltf::Parameter& param)
                        {
                            baseColorFactor.x = static_cast<float>(param.number_array.at(0));
                            baseColorFactor.y = static_cast<float>(param.number_array.at(1));
                            baseColorFactor.z = static_cast<float>(param.number_array.at(2));
                            baseColorFactor.w = static_cast<float>(param.number_array.at(3));
                        });
                        ExistFunc("metallicRoughnessTexture", [&](const tinygltf::Parameter& param)
                        {
                            metallicRoughnessTexture = static_cast<int>(param.json_double_value.at("index"));
                            metallicFactor = 1.0f;
                            roughnessFactor = 1.0f;
                        });
                        ExistFunc("metallicFactor", [&](const tinygltf::Parameter& param)
                        {
                            metallicFactor = static_cast<float>(param.number_value);
                        });
                        ExistFunc("roughnessFactor", [&](const tinygltf::Parameter& param)
                        {
                            roughnessFactor = static_cast<float>(param.number_value);
                        });
                        ExistFunc("normalTexture", [&](const tinygltf::Parameter& param)
                        {
                            normalTexture = static_cast<int>(param.json_double_value.at("index"));
                        });
                    }

                    int baseColorTexture{};
                    DirectX::XMFLOAT4 baseColorFactor{};
                    int metallicRoughnessTexture{};
                    float metallicFactor{};
                    float roughnessFactor{};
                    int normalTexture{};
                };

                /*
                glTFのTexture情報を持つ構造体
                */
                struct Texture
                {
                    Texture(const tinygltf::Texture& texture)
                    {
                        sampler = texture.sampler;
                        source = texture.source;
                    }
                    int source{};
                    int sampler{};
                };

                /*
                glTFのImage情報を持つ構造体
                */
                struct Image
                {
                    Image(const tinygltf::Image& image)
                    {
                        this->image = image.image;
                        width = image.width;
                        height = image.height;
                        component = image.component;
                    }
                    std::vector<unsigned char> image{};
                    int width{};
                    int height{};
                    int component{};
                };

                /*
                glTFの情報を持つ構造体
                バイナリデータ等すべてメモリ上に展開しているため取扱い注意
                */
                struct ModelData
                {
                    ModelData(const tinygltf::Model& model)
                    {
                        for (const auto& scene : model.scenes)
                            scenes.emplace_back(scene);
                        for (const auto& node : model.nodes)
                            nodes.emplace_back(node);
                        for (const auto& mesh : model.meshes)
                            meshes.emplace_back(mesh, model);
                        for (const auto& skin : model.skins)
                            skins.emplace_back(skin, model);
                        for (const auto& animation : model.animations)
                            animations.emplace_back(animation, model);
                        for (const auto& material : model.materials)
                            materials.emplace_back(material);
                        for (const auto& texture : model.textures)
                            textures.emplace_back(texture);
                        for (const auto& image : model.images)
                            images.emplace_back(image);
                        defaultScene = model.defaultScene;
                    }
                    std::vector<Scene> scenes{};
                    std::vector<Node> nodes{};
                    std::vector<Mesh> meshes{};
                    std::vector<Skin> skins{};
                    std::vector<Animation> animations{};
                    std::vector<Material> materials{};
                    std::vector<Texture> textures{};
                    std::vector<Image> images{};
                    int defaultScene{};
                };
            }
            using gr::Graphics::Mesh::Mesh;
            using gr::Graphics::Mesh::MeshBuilder;
            using gr::Graphics::Material::Material;
            using gr::Graphics::Material::MaterialBuilder;
            using gr::Graphics::Resource::Image::Image;
            using gr::Graphics::Resource::UploadBuffer::UploadBuffer;

            using gr::Graphics::Transform::Transform;
            class Node
            {
            public:
                const std::string& GetName() const { return name; }
                int GetMesh() const { return mesh; }
                int GetSkin() const { return skin; }
                int GetCamera() const { return camera; }
                const std::vector<int>& GetChildren() const { return children; }
                const Transform& GetTransform() const { return transform; }

                void SetTranslation(float x, float y, float z)
                {
                    transform.SetTranslation(x, y, z);
                }
                void SetTranslation(const DirectX::XMFLOAT3& translation)
                {
                    transform.SetTranslation(translation);
                }

                void SetRotation(float w, float x, float y, float z)
                {
                    transform.SetRotation(w, x, y, z);
                }
                void SetRotation(const DirectX::XMFLOAT4& rotation)
                {
                    transform.SetRotation(rotation);
                }

                void SetScale(float x, float y, float z)
                {
                    transform.SetScale(x, y, z);
                }
                void SetScale(const DirectX::XMFLOAT3& scale)
                {
                    transform.SetScale(scale);
                }
                Node(const glTF::Node& node)
                {
                    name = node.name;
                    camera = node.camera;
                    mesh = node.mesh;
                    skin = node.skin;
                    children = node.children;

                    transform = node.transform;
                }
            private:
                std::string name{};
                int mesh{};
                int skin{};
                int camera{};
                std::vector<int> children{};
                Transform transform{};
            };
            class Model
            {
            public:
                const std::vector<std::shared_ptr<Node>>& GetNodes() const
                {
                    return nodes;
                }
                const std::vector<int>& GetRootNodes() const
                {
                    return rootNodes;
                }
                const std::vector<std::vector<std::shared_ptr<Mesh>>>& GetMeshes() const
                {
                    return meshes;
                }
                const std::vector<std::shared_ptr<Material>>& GetMaterials() const
                {
                    return materials;
                }
                const std::vector<std::vector<int>>& GetMaterialIndices() const
                {
                    return materialIndices;
                }
                Model(ComPtr<ID3D12CommandAllocator> commandAllocator, ComPtr<ID3D12GraphicsCommandList> commandList, const glTF::ModelData& modelData)
                {
                    CreateMeshes(modelData);
                    CreateMaterials(commandAllocator, commandList, modelData);
                    CreateNodes(modelData);
                    rootNodes = modelData.scenes[0].nodes;
                }
            private:
                void CreateMeshes(const glTF::ModelData& modelData)
                {
                    for (const auto& mesh : modelData.meshes)
                    {
                        meshes.emplace_back();
                        materialIndices.emplace_back();
                        for (const auto& primitive : mesh.primitives)
                        {
                            auto builder = MeshBuilder();
                            auto indexCount = static_cast<UINT>(primitive.indices.elements.size());
                            builder
                                .CreateIndexBuffer(indexCount, sizeof(unsigned int)*indexCount, primitive.indices.elements.data());

                            std::vector<DirectX::XMFLOAT2> texcoords{};
                            std::vector<DirectX::XMFLOAT4> colors{};
                            auto vertexCount = primitive.positions.elements.size() / 3;
                            texcoords.resize(vertexCount);
                            colors.resize(vertexCount);
                            if (primitive.hasTexcoords)
                            {
                                memcpy(texcoords.data(), primitive.texcoords.elements.data(), sizeof(float)*primitive.texcoords.elements.size());
                            }
                            else
                            {
                                std::fill(texcoords.begin(), texcoords.end(), DirectX::XMFLOAT2{ 0.0f,0.0f });
                            }

                            if (primitive.hasColors)
                            {
                                memcpy(colors.data(), primitive.colors.elements.data(), sizeof(float)*primitive.colors.elements.size());
                            }
                            else
                            {
                                std::fill(colors.begin(), colors.end(), DirectX::XMFLOAT4{ 1.0f,1.0f, 1.0f, 1.0f });
                            }

                            //BinormalとTangentの算出
                            //TODO モデルに埋め込むとかなんとかする
                            //TODO UV持ってないモデルで詰むのもなんとかする
                            //TODO TriangleList以外で詰むのもなんとかする
                            std::vector<DirectX::XMFLOAT4> tangents{};
                            tangents.resize(vertexCount);
                            if (primitive.hasTangents)
                            {
                                memcpy(tangents.data(), primitive.tangents.elements.data(), sizeof(float)*primitive.tangents.elements.size());
                            }
                            else
                            {
                                if (!primitive.hasTexcoords)
                                {
                                    panic("model didn't have tangents and texcoords \n cannot calclate tangents");
                                }
                                DirectX::XMFLOAT4 tangent{};
                                std::array<DirectX::XMFLOAT3, 3> pos{};
                                std::array<DirectX::XMFLOAT2, 3> tex{};
                                std::array<unsigned int, 3> indices{};
                                for (int i = 0; i < static_cast<int>(indexCount); i += 3)
                                {
                                    indices[0] = primitive.indices.elements[i + 0].uintData;
                                    indices[1] = primitive.indices.elements[i + 1].uintData;
                                    indices[2] = primitive.indices.elements[i + 2].uintData;

                                    pos[0].x = primitive.positions.elements[indices[0] * 3 + 0].floatData;
                                    pos[0].y = primitive.positions.elements[indices[0] * 3 + 1].floatData;
                                    pos[0].z = primitive.positions.elements[indices[0] * 3 + 2].floatData;

                                    pos[1].x = primitive.positions.elements[indices[1] * 3 + 0].floatData;
                                    pos[1].y = primitive.positions.elements[indices[1] * 3 + 1].floatData;
                                    pos[1].z = primitive.positions.elements[indices[1] * 3 + 2].floatData;

                                    pos[2].x = primitive.positions.elements[indices[2] * 3 + 0].floatData;
                                    pos[2].y = primitive.positions.elements[indices[2] * 3 + 1].floatData;
                                    pos[2].z = primitive.positions.elements[indices[2] * 3 + 2].floatData;

                                    tex[0].x = primitive.texcoords.elements[indices[0] * 2 + 0].floatData;
                                    tex[0].y = primitive.texcoords.elements[indices[0] * 2 + 1].floatData;

                                    tex[1].x = primitive.texcoords.elements[indices[1] * 2 + 0].floatData;
                                    tex[1].y = primitive.texcoords.elements[indices[1] * 2 + 1].floatData;

                                    tex[2].x = primitive.texcoords.elements[indices[2] * 2 + 0].floatData;
                                    tex[2].y = primitive.texcoords.elements[indices[2] * 2 + 1].floatData;

                                    CalcTangent(pos, tex, &tangent);

                                    tangents[indices[0]] = tangent;
                                    tangents[indices[1]] = tangent;
                                    tangents[indices[2]] = tangent;

                                }
                            }

                            meshes.back().emplace_back(
                                builder
                                .CreateVertexBuffer("POSITION", sizeof(DirectX::XMFLOAT3), sizeof(float)*primitive.positions.elements.size(), primitive.positions.elements.data())
                                .CreateVertexBuffer("NORMAL", sizeof(DirectX::XMFLOAT3), sizeof(float)*primitive.normals.elements.size(), primitive.normals.elements.data())
                                .CreateVertexBuffer("TEXCOORD", sizeof(DirectX::XMFLOAT2), sizeof(DirectX::XMFLOAT2)*texcoords.size(), texcoords.data())
                                .CreateVertexBuffer("COLOR", sizeof(DirectX::XMFLOAT4), sizeof(DirectX::XMFLOAT4)*colors.size(), colors.data())
                                .CreateVertexBuffer("TANGENT", sizeof(DirectX::XMFLOAT4), sizeof(DirectX::XMFLOAT4)*tangents.size(), tangents.data())
                                .Build()
                            );
                            materialIndices.back().emplace_back(primitive.material);
                        }
                    }
                }
                //参考サイト : http://www.opengl-tutorial.org/jp/intermediate-tutorials/tutorial-13-normal-mapping/#%E6%8E%A5%E7%B7%9A%E3%81%A8%E5%BE%93%E6%8E%A5%E7%B7%9A%E3%81%AE%E8%A8%88%E7%AE%97
                void CalcTangent(
                    const std::array<DirectX::XMFLOAT3, 3>& positions,
                    const std::array<DirectX::XMFLOAT2, 3>& texcoords,
                    DirectX::XMFLOAT4* tangent
                )
                {
                    auto p0 = DirectX::XMLoadFloat3(&positions[0]);
                    auto p1 = DirectX::XMLoadFloat3(&positions[1]);
                    auto p2 = DirectX::XMLoadFloat3(&positions[2]);

                    auto t0 = DirectX::XMLoadFloat2(&texcoords[0]);
                    auto t1 = DirectX::XMLoadFloat2(&texcoords[1]);
                    auto t2 = DirectX::XMLoadFloat2(&texcoords[2]);

                    DirectX::XMFLOAT3 deltaP1{};
                    DirectX::XMFLOAT3 deltaP2{};
                    DirectX::XMFLOAT2 deltaT1{};
                    DirectX::XMFLOAT2 deltaT2{};

                    DirectX::XMStoreFloat3(&deltaP1, DirectX::XMVectorSubtract(p1, p0));
                    DirectX::XMStoreFloat3(&deltaP2, DirectX::XMVectorSubtract(p2, p1));

                    DirectX::XMStoreFloat2(&deltaT1, DirectX::XMVectorSubtract(t1, t0));
                    DirectX::XMStoreFloat2(&deltaT2, DirectX::XMVectorSubtract(t2, t1));

                    float r = 1.0f / (deltaT1.x*deltaT2.y - deltaT1.y*deltaT2.x);

                    tangent->x = (deltaP2.x*deltaT1.x - deltaP1.x*deltaT2.x)*r;
                    tangent->y = (deltaP2.y*deltaT1.x - deltaP1.y*deltaT2.x)*r;
                    tangent->z = (deltaP2.z*deltaT1.x - deltaP1.z*deltaT2.x)*r;

                    auto lt = sqrtf(tangent->x*tangent->x + tangent->y*tangent->y + tangent->z*tangent->z);
                    tangent->x /= lt;
                    tangent->y /= lt;
                    tangent->z /= lt;

                    tangent->w = 1.0f;
                }
                void CreateMaterials(ComPtr<ID3D12CommandAllocator> commandAllocator, ComPtr<ID3D12GraphicsCommandList> commandList, const glTF::ModelData& modelData)
                {
                    std::array<unsigned char, 4> color{};
                    color = { 0xff,0xff,0xff,0xff };
                    imageWhiteSrgb = std::make_shared<Image>(commandAllocator, commandList, 1, 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, color.data());
                    imageWhite = std::make_shared<Image>(commandAllocator, commandList, 1, 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, color.data());
                    color = { 0x0,0x0,0x0,0x0 };
                    imageBlack = std::make_shared<Image>(commandAllocator, commandList, 1, 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, color.data());
                    color = { 0x80,0x80,0xff,0xff };
                    imageNormal = std::make_shared<Image>(commandAllocator, commandList, 1, 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, color.data());

                    images.resize(modelData.images.size());
                    for (const auto& material : modelData.materials)
                    {
                        auto builder = MaterialBuilder();
                        if (auto idx = material.baseColorTexture; idx != -1)
                        {
                            if (images[idx] == nullptr)
                            {
                                const auto& image = modelData.images[idx];
                                images[idx] = std::make_shared<Image>(commandAllocator, commandList, static_cast<UINT64>(image.width), static_cast<UINT>(image.height), 4, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, image.image.data());
                                builder.SetBaseColorImage(images[idx]);
                            }
                        }
                        else
                        {
                            builder.SetBaseColorImage(imageWhiteSrgb);
                        }

                        if (auto idx = material.metallicRoughnessTexture; idx != -1)
                        {
                            if (images[idx] == nullptr)
                            {
                                const auto& image = modelData.images[idx];
                                images[idx] = std::make_shared<Image>(commandAllocator, commandList, static_cast<UINT64>(image.width), static_cast<UINT>(image.height), 4, DXGI_FORMAT_R8G8B8A8_UNORM, image.image.data());
                                builder.SetMetallicRoughnessImage(images[idx]);
                            }
                        }
                        else
                        {
                            builder.SetMetallicRoughnessImage(imageWhite);
                        }

                        if (auto idx = material.normalTexture; idx != -1)
                        {
                            if (images[idx] == nullptr)
                            {
                                const auto& image = modelData.images[idx];
                                images[idx] = std::make_shared<Image>(commandAllocator, commandList, static_cast<UINT64>(image.width), static_cast<UINT>(image.height), 4, DXGI_FORMAT_R8G8B8A8_UNORM, image.image.data());
                                builder.SetNormalImage(images[idx]);
                            }
                        }
                        else
                        {
                            builder.SetNormalImage(imageNormal);
                        }
                        builder
                            .SetBaseColorFactor(material.baseColorFactor)
                            .SetMetallic(material.metallicFactor)
                            .SetRoughness(material.roughnessFactor);

                        materials.emplace_back(builder.Build());
                    }
                }
                void CreateNodes(const glTF::ModelData& modelData)
                {
                    for (const auto& node : modelData.nodes)
                    {
                        nodes.emplace_back(std::make_shared<Node>(node));
                    }
                }
                std::vector<std::vector<std::shared_ptr<Mesh>>> meshes{};
                std::vector<std::vector<int>> materialIndices{};
                std::vector<std::shared_ptr<Material>> materials{};
                std::vector<std::shared_ptr<Image>> images{};
                std::vector<std::shared_ptr<Node>> nodes{};
                std::vector<int> rootNodes{};

                std::shared_ptr<Image> imageWhiteSrgb{};
                std::shared_ptr<Image> imageWhite{};
                std::shared_ptr<Image> imageBlack{};
                std::shared_ptr<Image> imageNormal{};
            };

            class ModelReference
            {
            public:
                std::shared_ptr<UploadBuffer> GetNodeBuffer() const
                {
                    return nodeBuffer;
                }
                ModelReference(std::shared_ptr<Model> model) :
                    model(model)
                {
                    int nodeCount = static_cast<int>(model->GetNodes().size());
                    nodeMatrices.resize(nodeCount);

                    //アライメント256に揃える
                    nodeCount += 4 - (nodeCount % 4);
                    nodeBuffer = std::make_shared<UploadBuffer>(nodeCount * sizeof(DirectX::XMFLOAT4X4));
                    Update();
                }
                void Update()
                {
                    std::vector<DirectX::XMMATRIX> matrices{};
                    int nodeCount = static_cast<int>(model->GetNodes().size());
                    for (int i = 0; i < nodeCount; i++)
                    {
                        auto m = model->GetNodes()[i]->GetTransform().CalclateMatrix();
                        matrices.emplace_back(DirectX::XMLoadFloat4x4(&m));
                    }
                    std::function<void(int, DirectX::XMMATRIX)> fixNoedeTransform = [&](int nodeIndex, DirectX::XMMATRIX parentMatrix)
                    {
                        matrices[nodeIndex] = matrices[nodeIndex] * parentMatrix;
                        DirectX::XMStoreFloat4x4(&nodeMatrices[nodeIndex], matrices[nodeIndex]);
                        for (auto child : model->GetNodes()[nodeIndex]->GetChildren())
                        {
                            fixNoedeTransform(child, matrices[nodeIndex]);
                        }
                    };
                    for (auto node : model->GetRootNodes())
                    {
                        fixNoedeTransform(node, DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f));
                    }
                    {
                        auto ptr = nodeBuffer->Map();
                        memcpy(ptr, nodeMatrices.data(), nodeMatrices.size() * sizeof(DirectX::XMFLOAT4X4));
                        nodeBuffer->Unmap();
                    }
                }
            private:
                std::shared_ptr<UploadBuffer> nodeBuffer{};
                std::vector<DirectX::XMFLOAT4X4> nodeMatrices{};
                std::shared_ptr<Model> model{};

                ComPtr<ID3D12Device> device{};
            };
        }
    }
}
