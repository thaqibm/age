//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_AGEMANAGER_H
#define A4_AGEMANAGER_H


#include <iostream>

#include <cmath>
#include <memory>
#include <vector>
#include <set>
#include <cmath>
#include <string>

#include "ColliderSystem.h"
#include "RenderSystem.h"
#include "EntityManager.h"
#include "prefab.h"

#include "components/componentManager.h"
#include "systems/SystemManager.h"
#include "components/default/default.h"
#include "util/nthType.h"

constexpr const int rows = 25;
constexpr const int cols = 80;
constexpr const int nStatus = 3;

class AGEManager {
private:

    using State = RenderSystem::State;
    using prefabName = const char*;
    using entityName = const char*;
    /*
     * Owns a relationship with Entity, Component, System managers
     * */
    std::unique_ptr<EntityManager> mEntity_Manager;
    std::unique_ptr<componentManager> mComponent_Manager;
    std::unique_ptr<SystemManager> mSystem_Manager;
    std::unique_ptr<RenderSystem> mRender_system;
    std::unique_ptr<ColliderSystem> mCollider_system;
    std::unordered_map<prefabName, std::set<EntityID>> mPrefab_map;
    std::unordered_map<entityName, EntityID> mEntityName_map;

public:
    AGEManager():
    mEntity_Manager{std::make_unique<EntityManager>()},
    mComponent_Manager{std::make_unique<componentManager>()},
    mSystem_Manager{std::make_unique<SystemManager>()},
    mRender_system{std::make_unique<RenderSystem>(25, 80, mComponent_Manager.get(), mEntity_Manager.get())},
    mCollider_system{std::make_unique<ColliderSystem>(mComponent_Manager.get(), mEntity_Manager.get())},
    mPrefab_map(),
    mEntityName_map()
    {
        mRender_system->init();
        mCollider_system->init();
   }
    EntityID createEntity(){
        return mEntity_Manager->CreateEntity();
    }
    EntityID createEntity(entityName name){
        auto e =  mEntity_Manager->CreateEntity();
        mEntityName_map.insert({name, e});
        return e;
    }

    template<typename Last>
    void attachComponents(EntityID id){
        attachComponent(id, Last{});
    }

    template<typename First, typename Second,typename... Rest>
    void attachComponents(EntityID id){
        attachComponent(id, First{});
        attachComponents<Second, Rest...>(id);
    }

    template<typename... Args>
    EntityID createEntityFromPrefab(Prefab<Args...> prefab){
        auto e = mEntity_Manager->CreateEntity();
        // init with default values
        attachComponents<Args...>(e);
        add_data_prefab<0, Args...>(e, prefab);
        mPrefab_map[prefab.name].insert(e);
        return e;
    }

    template<typename... Args>
    EntityID createEntityFromPrefab(Prefab<Args...> prefab, entityName name){
        auto e = mEntity_Manager->CreateEntity();
        // init with default values
        attachComponents<Args...>(e);
        add_data_prefab<0, Args...>(e, prefab);
        mPrefab_map[prefab.name].insert(e);
        mEntityName_map.insert({name, e});
        return e;
    }

    const std::set<EntityID>& getEntityWithPrefab(prefabName name){
        return mPrefab_map.at(name);
    }

    EntityID createDrawableEntity(AGE_COMPONENTS::Position p, AGE_COMPONENTS::Drawable d){
        auto e0 = mEntity_Manager->CreateEntity();
        attachComponent<AGE_COMPONENTS::Position>(e0, p);
        attachComponent<AGE_COMPONENTS::Drawable>(e0, d);
        return e0;
    }
    void destroyEntity(EntityID id){
        mEntity_Manager->DestroyEntity(id);
        mComponent_Manager->DestroyEntity(id);
        mSystem_Manager->DestroyEntity(id);
    }

    template<typename T>
    void addComponent(){
        mComponent_Manager->registerComponent<T>();
    }

    template<typename T>
    void safe_addComponent(){
        if(mComponent_Manager->componentAttached<T>()){
            return ;
        }
        mComponent_Manager->registerComponent<T>();
    }

    template<typename T> bool hasSystem(){
        return mSystem_Manager->hasSystem<T>();
    }

    template<typename T>
    void attachComponent(EntityID id, T component){
        mComponent_Manager->AttachComponent<T>(id, component);

        auto componentsSign = mEntity_Manager->getSignature(id);
        componentsSign.set(mComponent_Manager->getComponentId<T>(), 1);
        mEntity_Manager->setEntitySignature(id, componentsSign);
        mSystem_Manager->NotifySystems(id, componentsSign);
    }

    template<typename T>
    void detachComponent(EntityID id){
        mComponent_Manager->DetachComponent<T>(id);
        auto componentsSign = mEntity_Manager->getSignature(id);
        componentsSign.set(mComponent_Manager->getComponentId<T>(), 0);
        mEntity_Manager->setEntitySignature(id, componentsSign);
        mSystem_Manager->NotifySystems(id, componentsSign);
    }


    template<typename T>
    T& getComponentData(EntityID id){
        return mComponent_Manager->getComponentData<T>(id);
    }

    template<typename T>
    ComponentID getComponentID(){
        return mComponent_Manager->getComponentId<T>();
    }

    template<typename T>
    std::shared_ptr<T> registerSystem(){
        return mSystem_Manager->registerSystem<T>(mComponent_Manager.get(), mEntityName_map);
    }

    template<typename Sys, typename Last>
    void SetSystemTraits(){
        ComponentSignature csignature = mSystem_Manager-> getSystemSignature<Sys>();
        csignature.set(mComponent_Manager->getComponentId<Last>(), 1);
        mSystem_Manager->setSignature<Sys>(csignature);
    }
    template<typename Sys, typename First, typename Second,typename... Rest>
    void SetSystemTraits(){
        ComponentSignature csignature = mSystem_Manager-> getSystemSignature<Sys>();
        csignature.set(mComponent_Manager->getComponentId<First>(), 1);
        mSystem_Manager->setSignature<Sys>(csignature);
        SetSystemTraits<Sys, Second, Rest...>();
    }

    template<typename T>
    ComponentSignature getSystemSignature(){
        return mSystem_Manager->getSystemSignature<T>();
    }

    inline const State& getState() const{
        return mRender_system->getState();
    }
    void setStatusLine(size_t n, const std::string& s){
        mRender_system->setStatusLine(n, s);
    }
public:
    void update(float dt, int ch){
        /**
         * Update cycle:
         * - Detect Collisions
         * - Get input
         * - Pass collision data to all systems
         * -Pass input to all systems
         * - Call update on each system
         * - Render
         **/
        for(auto& [n, s] : mSystem_Manager->getSystemMap()){
            const auto& collisionData = mCollider_system->detectCollisions();
            s->Collisions.setNameMap(mEntityName_map);
            s->Collisions.setCollisionData(collisionData);
            s->Input.setInput(ch);
            s->DeltaTime.setDt(dt);
            s->Bounds = {cols - 2 - nStatus,rows - 2 };
            s->StatusLines.setRenderSystem(mRender_system.get());
            s->EntityData.setMgr(mEntity_Manager.get());
            s->EntityData.setNameMap(mEntityName_map);
            if(!s->update()){
                end();
            }
            mCollider_system->resetCollisions();
        }
        mRender_system->renderDrawable();
    }
    void end(){
        for(auto& [n, s] : mSystem_Manager->getSystemMap()){
            s->end_base();
        }
    }


private:
    template<typename T>
    void add_data(EntityID e, T t){
        auto& data = getComponentData<T>(e);
        data = t;
    }
    template<int N,typename... Args>
    void add_data_prefab(EntityID id, Prefab<Args...> prefab){
        if constexpr (N < sizeof...(Args)){
            auto tn = std::get<N>(prefab.data);
            add_data(id, tn);
            add_data_prefab<N+1, Args...>(id, prefab);
        }
    }


};


#endif //A4_AGEMANAGER_H