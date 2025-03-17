#include "ECS.h"

ISystem::~ISystem() {}

SystemManager::~SystemManager()
{
    for (auto& pair : m_systems) {
        printf("Freeing System %s\n", pair.first.c_str());
        delete pair.second;
    }
}

void SystemManager::addSystem(const std::string& id, ISystem *system)
{
    if (hasSystem(id))
    {
        printf("Already a System with ID=%s", id.c_str());
    }
    else
    {
        m_systems[id] = system;
        system->start();
    }
}

bool SystemManager::hasSystem(const std::string& id) const
{
    return m_systems.find(id) != m_systems.end();
}

void SystemManager::removeSystem(const std::string& id)
{
    if (hasSystem(id))
    {
        m_systems[id]->end();
        m_systems.erase(id);
    }
}

void SystemManager::update(int time) const
{
    for (auto it = m_systems.begin(); it != m_systems.end(); ++it)
    {
        it->second->update(time);
    }
}
