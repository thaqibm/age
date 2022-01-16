//
// Created by thaqi on 12/8/2021.
//

#ifndef A4_COMPONENTMANAGER_H
#define A4_COMPONENTMANAGER_H


#include <unordered_map>
#include <memory>
#include <utility>
#include "../EntityManager.h"
#include "componentDataArray.h"
#include "component.h"



class componentManager {
private:
    using ComponentName = const char*;
    ComponentID next{};
    std::unordered_map<ComponentName, std::shared_ptr<DataArray>> nameToDataArrayMap;
    std::unordered_map<ComponentName, ComponentID> nameToTpeMap;

    inline bool componentExists(ComponentName name){
        return nameToTpeMap.find(name) != nameToTpeMap.end();
    }
    template<typename T> std::shared_ptr<componentDataArray<T>> getComponentDataArray(){
        auto name = typeid(T).name();
        assert(componentExists(name) && "Component DNE");
        return std::static_pointer_cast<componentDataArray<T>>(nameToDataArrayMap[name]);
    }
public:
    template<typename T> void registerComponent(){
        auto tpe = typeid(T).name();
        assert(!componentExists(tpe) && "Component already reg");

        nameToTpeMap[tpe] = next, next++;
        nameToDataArrayMap[tpe] = std::make_shared<componentDataArray<T>>();
    }

    template<typename T> bool componentAttached(){
        return componentExists(typeid(T).name());
    }
    template<typename T> ComponentID getComponentId(){
        auto name = typeid(T).name();
        assert(componentExists(name) && "Component DNE");
        return nameToTpeMap[name];
    }
    ComponentID getComponentId(const char* name){
        assert(componentExists(name) && "Component DNE" && name);
        return nameToTpeMap[name];
    }


    template<typename T>
    void AttachComponent(EntityID id, T component){
        getComponentDataArray<T>()->attachComponent(id, component);
    }
    template<typename T>
    void DetachComponent(EntityID id){
        getComponentDataArray<T>()->detachComponent(id);
    }
    template<typename T>
    T& getComponentData(EntityID id){
        return getComponentDataArray<T>()->getData(id);
    }

    void DestroyEntity(EntityID id){
        for(auto const& p : nameToDataArrayMap){
            (p.second)->DestroyEntity(id);
        }
    }
};


#endif //A4_COMPONENTMANAGER_H
