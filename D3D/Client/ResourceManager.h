#pragma once

class Material;
class Mesh;
class Shader;
class Texture;
class Model;


class ResourceManager
{
public:
    static ResourceManager* GetInstance()
    {
        static ResourceManager instance;
        return &instance;
    }

    template<typename T, typename... Args>
    shared_ptr<T> Load(const wstring& path, Args&&... args);

    template<typename T>
    shared_ptr<T> Get(const wstring& path);

    template<typename T>
    void Add(const wstring& path, shared_ptr<T> object);

private:
    unordered_map<wstring, shared_ptr<Shader>> _shaderMap;
    unordered_map<wstring, shared_ptr<Mesh>> _meshMap;
    unordered_map<wstring, shared_ptr<Texture>> _textureMap;
    unordered_map<wstring, shared_ptr<Model>> _modelMap;

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

// Add �Լ�
template<typename T>
inline void ResourceManager::Add(const wstring& path, shared_ptr<T> object)
{
    GetResourceMap<T>()[path] = object;
}

// Get �Լ�
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
    if constexpr (is_same_v<T, Shader>)
    {
        return _shaderMap;
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
    else
    {
        throw std::invalid_argument("Unsupported resource type");
    }
}