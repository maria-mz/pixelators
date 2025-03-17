#ifndef ECS_H
#define ECS_H

#include <any>
#include <map>
#include <vector>
#include <string>
#include <typeindex>
#include <unordered_map>

// Entity Component System (ECS) Implementation
// Based on concepts discussed in the blog post by Richard Lord:
//      https://www.richardlord.net/blog/ecs/what-is-an-entity-framework


class Entity
{
    public:
        template <typename T>
        void addComponent(T* component)
        {
            std::type_index index(typeid(T));
            m_components[index] = std::any(component);
        }

        template <typename T>
        void removeComponent()
        {
            std::type_index index(typeid(T));
            m_components.erase(index);
        }

        template <typename T>
        bool hasComponent() const
        {
            std::type_index index(typeid(T));
            return m_components.find(index) != m_components.end();
        }

        template <typename T>
        T* getComponent()
        {
            std::type_index index(typeid(T));

            auto it = m_components.find(index);

            if (it != m_components.end())
            {
                return std::any_cast<T*>(it->second);
            }

            return nullptr;
        }

    private:
        std::unordered_map<std::type_index, std::any> m_components;
};

class ISystem
{
    public:
        virtual ~ISystem() = 0;
        virtual bool start() = 0;
        virtual void update(int time) = 0;
        virtual void end() = 0;
        void addEntity(Entity* e) { m_entities.push_back(e); };

    protected:
        std::vector<Entity*> m_entities;
};

class SystemManager
{
    public:
        ~SystemManager();
        void addSystem(const std::string& id, ISystem *system);
        bool hasSystem(const std::string& id) const;
        void update(int time) const;
        void removeSystem(const std::string& id);

    private:
        std::map<std::string, ISystem*> m_systems;
};

#endif