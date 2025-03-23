#ifndef SYSTEM_MANAGER_H_
#define SYSTEM_MANAGER_H_

#include <map>
#include <string>
#include "SDL2/SDL.h"
#include "Systems/ISystem.h"

// System manager idea based on concepts discussed in the blog post by Richard Lord:
//      https://www.richardlord.net/blog/ecs/what-is-an-entity-framework

class SystemManager
{
    public:
        ~SystemManager();
        void addSystem(const std::string& id, ISystem *system);
        bool hasSystem(const std::string& id) const;
        void input(SDL_Event &e);
        void update(int time) const;
        void removeSystem(const std::string& id);

    private:
        std::map<std::string, ISystem*> m_systems;
};

#endif