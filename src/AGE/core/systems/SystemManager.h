//
// Created by thaqi on 12/8/2021.
//

#ifndef A4_SYSTEMMANAGER_H
#define A4_SYSTEMMANAGER_H

#include <unordered_map>
#include <bitset>
#include <set>
#include <memory>
#include <cassert>
#include "../EntityManager.h"
#include "../components/componentManager.h"
#include "System.h"

class SystemManager {
    using SystemName = const char*;
private:
    std::unordered_map<SystemName, ComponentSignature> SignaturesMap{};
    std::unordered_map<SystemName, std::shared_ptr<System_base>> SystemsMap{};
    inline bool systemExists(SystemName sysname){
        return SignaturesMap.find(sysname) != SignaturesMap.end();
    }
public:
    template<typename T>
    std::shared_ptr<T> registerSystem(componentManager* ctx, const std::unordered_map<const char*, EntityID>& nm){
        static_assert(std::is_base_of_v<System_base, T>);

        SystemName sysName = typeid(T).name();
        assert(!systemExists(sysName) && "System already reg");
        auto shr = std::make_shared<T>();
        shr->ComponentData.setCtx(ctx);
        shr->Collisions.setNameMap(nm);
        SystemsMap.insert({sysName, shr});
        return shr;
    }
    template<typename T>
    bool hasSystem(){
        return systemExists(typeid(T).name());
    }
    template<typename T>
    void setSignature(ComponentSignature signature){
        auto sysName = typeid(T).name();
        assert(systemExists(sysName) && "System DNE");
        SignaturesMap[sysName] = signature;
    }

    void DestroyEntity(EntityID id){
        for(auto const& p : SystemsMap){
            p.second->Entities.erase(id);
        }
    }

    inline auto& getSystemMap(){
        return SystemsMap;
    }
    void NotifySystems(EntityID id, ComponentSignature signature){
        for(auto const& p : SystemsMap){
            auto sysName = p.first;
            auto sys = p.second;
            auto sysSignature = SignaturesMap[sysName];

            /**
             * Entity contains all the compoennets needed for the system
             * Example:
             * Entity 0, signature: 0101100000
             * Physics system requirement: 0101000000
             * Result of bitwise &: 0101100000 & 0101000000 = 0101000000
             * SO entity 0 can be handled my Physics System
             * */
            if((signature & sysSignature) == sysSignature){
                sys->Entities.insert(id);
            }
            else{
                sys->Entities.erase(id);
            }
        }
    }

    template<typename T>
    ComponentSignature getSystemSignature(){
        return SignaturesMap[typeid(T).name()];
    }

};


#endif //A4_SYSTEMMANAGER_H
