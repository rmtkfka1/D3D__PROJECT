#pragma once

class Material;
class Mesh;
class GraphicsShader;
class ComputeShader;
class Texture;
class Model;
class GameObject;


class ResourceManager
{
public:
    static ResourceManager* GetInstance()
    {
        static ResourceManager instance;
        return &instance;
    }

    void Init();
    void CreateDefaultMesh();
    void CreateDefaultShader();
    void CreateDefaultMaterial();

    template<typename T, typename... Args>
    shared_ptr<T> Load(const wstring& path, Args&&... args);

    template<typename T>
    shared_ptr<T> Get(const wstring& path);

    template<typename T>
    void Add(const wstring& key, shared_ptr<T> object);

private:
    unordered_map<wstring, shared_ptr<GraphicsShader>> _graphicsShaderMap;
    unordered_map<wstring, shared_ptr<ComputeShader>> _computeShaderMap;
    unordered_map<wstring, shared_ptr<Mesh>> _meshMap;
    unordered_map<wstring, shared_ptr<Texture>> _textureMap;
    unordered_map<wstring, shared_ptr<Model>> _modelMap;
    unordered_map<wstring, shared_ptr<Material>> _materialMap;
    unordered_map<wstring, shared_ptr<GameObject>> _gameObjectMap;

    template<typename T>
    unordered_map<wstring, shared_ptr<T>>& GetResourceMap();
};

template<typename T, typename... Args>
inline shared_ptr<T> ResourceManager::Load(const wstring& path, Args&&... args)
{
   
    auto& Map = GetResourceMap<T>();

    auto it = Map.find(path);
    if (it != Map.end())
        return it->second;

    shared_ptr<T> object = make_shared<T>();
    object->Init(path,forward<Args>(args)...);
    Map[path] = object;
    return object;

}

// Add 함수
template<typename T>
inline void ResourceManager::Add(const wstring& key, shared_ptr<T> object)
{
    GetResourceMap<T>()[key] = object;
}

// Get 함수
template<typename T>
inline shared_ptr<T> ResourceManager::Get(const wstring& key)
{
    auto it = GetResourceMap<T>().find(key);
    if (it != GetResourceMap<T>().end())
        return it->second;

  
    return nullptr; 
}

template<typename T>
unordered_map<wstring, shared_ptr<T>>& ResourceManager::GetResourceMap()
{
    if constexpr (is_same_v<T, GraphicsShader>)
    {
        return _graphicsShaderMap;
    }
    else if constexpr (is_same_v<T, ComputeShader>)
    {
        return _computeShaderMap;
    }
    else if constexpr (is_same_v<T, Mesh>)
    {
        return _meshMap;
    }
    else if constexpr (is_same_v<T, Texture>)
    {
        return _textureMap;
    }
    else if constexpr (is_same_v<T, Model>)
    {
        return _modelMap;
    }
    else if constexpr (is_same_v<T, Material>)
    {
        return _materialMap;
    }
    else if constexpr (is_same_v<T, GameObject>)
    {
        return _gameObjectMap;
    }
    else
    {
        throw std::invalid_argument("Unsupported resource type");
    }
}