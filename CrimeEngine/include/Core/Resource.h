#pragma once
#include "Core/Core.h"

namespace ce
{
    enum class ResourceType
    {
        None,
        Texture,
		Shader,
        Mesh,
        Audio,
        Font,
		Map
    };

    class Resource
    {
        friend class ResourceManager;
    public:
        virtual ~Resource() = default;

        const ResourceId& GetID() const { return m_id; }
        const std::string& GetPath() const { return m_path; }
        const std::string& GetFileName() const { return m_fileName; }
        ResourceType GetType() const { return m_type; }
        bool IsLoaded() const { return m_isLoaded; }

        bool m_isReloaded = false;

         virtual void Reload() { };
    protected:
        Resource(ResourceType a_type)
            : m_type(a_type),
            m_path(""),
            m_fileName(""),
            m_id(0) { }

        virtual void OnInitialize() { };

        ResourceType m_type;
        std::string m_path;
        std::string m_fileName;
    private:
        //Resource should only be created by the resource manager.
        void Create(ResourceId a_id, const std::string& a_path, const std::string& a_fileName)
        {
            m_id = a_id;
            m_path = a_path;
            m_fileName = a_fileName;
            m_isLoaded = true;
        }

        ResourceId m_id;
        bool m_isLoaded {  false };
    };
}