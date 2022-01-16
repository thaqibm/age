//
// Created by thaqi on 12/8/2021.
//

#ifndef A4_COMPONENTDATAARRAY_H
#define A4_COMPONENTDATAARRAY_H

#include "../EntityManager.h"
#include "component.h"

#include <array>
#include <unordered_map>
#include <cassert>


class DataArray{
public:
    virtual void DestroyEntity(EntityID entityId) = 0;
    virtual ~DataArray() = default;
};

template<typename T> class componentDataArray: public DataArray {
public:
    /**
     * attaches component of Type T to entity with id entityId
     */
    void attachComponent(EntityID entityId, T component) {
        assert(n < MAX_ENTT && "Maximum number of entities reached");
        entityMap[entityId] = n;
        idxMap[n] = entityId;
        entityData[n++] = component;
    }

    void detachComponent(EntityID entityId){
        assert(entityExists(entityId));

        // move entityId to last element
        // keeps the array tighely packed.
        // idea from:
        // https://austinmorlan.com/posts/entity_component_system/

        auto idx = idxMap[entityId];
        auto lst = n-1;
        entityData[idx] = entityData[lst];

        auto lstEnttId = idxMap[lst];
        entityMap[lstEnttId] = idx;
        idxMap[idx] = lstEnttId;

        idxMap.erase(lst);
        entityMap.erase(entityId);
        --n;
    }

    T& getData(EntityID id){
        assert(( "Entity Not found", entityExists(id)));
        return entityData[entityMap[id]];
    }
    void DestroyEntity(EntityID entityId) override {
        if(entityExists(entityId)){
            detachComponent(entityId);
        }
    }
private:
    /**
     * entityData: Maps each entity id -> component data for type T
     * Ex:
     * componentDataArray<Transform> c.entityData[0] : stores the Transform data for entity 0.
     */
    std::array<T, MAX_ENTT> entityData;
    std::unordered_map<EntityID, size_t> entityMap;
    std::unordered_map<size_t, EntityID> idxMap;
    size_t n; // current number of entities
    inline bool entityExists(EntityID id){
        return entityMap.find(id) != entityMap.end();
    }
};


#endif //A4_COMPONENTDATAARRAY_H
