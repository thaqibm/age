//
// Created by thaqi on 12/8/2021.
//

#ifndef A4_ENTITYMANAGER_H
#define A4_ENTITYMANAGER_H

#include <cstdint>
#include <array>
#include <cassert>
#include <queue>
#include <set>
#include "components/component.h"


/*
 * Aliases for entities and the maximum number of entities stored currently
 * */
using EntityID = std::uint32_t;
static const constexpr std::size_t MAX_ENTT = 5000;

class EntityManager {
    struct Entity{
        bool isAlive{};
        ComponentSignature signature;
    };
private:
    std::queue<EntityID> Available_Entities;
    std::array<Entity, MAX_ENTT> current_Entt;
    std::set<EntityID> Entities;
    size_t nEntt;
public:
    EntityManager(){
        for(EntityID i = 0; i<MAX_ENTT; ++i)
            Available_Entities.push(i);
        nEntt = 0;
    }

    const std::set<EntityID>& getEntities(){
        return Entities;
    }
    EntityID CreateEntity(){
        EntityID id = Available_Entities.front();
        current_Entt[id].signature.reset();
        current_Entt[id].isAlive = true;
        Available_Entities.pop();
        Entities.insert(id);
        ++nEntt;
        return id;
    }

    inline Entity getEntity(size_t i){
        assert(Entities.find(i) != Entities.end());
        return current_Entt[i];
    }

    inline size_t size(){
        return nEntt;
    }

    void DestroyEntity(EntityID id){
        current_Entt[id].signature.reset();
        current_Entt[id].isAlive = false;
        Entities.erase(id);
        Available_Entities.push(id);
        --nEntt;
    }

    void setEntitySignature(EntityID id, ComponentSignature signature){
        current_Entt[id].signature = signature;
    }

    inline ComponentSignature getSignature(EntityID id){
        return current_Entt[id].signature;
    }
};




#endif //A4_ENTITYMANAGER_H
