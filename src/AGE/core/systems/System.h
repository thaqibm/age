//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_SYSTEM_H
#define A4_SYSTEM_H
#include "../components/componentManager.h"
#include "../RenderSystem.h"

#include <cassert>
#include <string>
using EntityID = std::uint32_t;



struct System_base{
private:
    struct Bounds final {
        int x;
        int y;
    };
    struct ComponentData final {
        void setCtx(componentManager* c){
            ctx = c;
        }
        template<typename T>
        T& getComponentData(EntityID eid){
            return ctx->getComponentData<T>(eid);
        }
    private:
        componentManager* ctx;
    };
    struct Input final{
        inline bool GetKey(int k){
            return k == input;
        }
        inline void setInput(int kin){
            input = kin;
        }
    private:
        int input;
    };

    struct DeltaTime final {
        inline void setDt(float td){
            dt = td;
        }
        float operator()() const {
            return dt;
        }
    private:
        float dt;
    };
    struct Collisions final{
        using entityName = const char*;
        using CollisionData = std::pair<EntityID , EntityID>;

        CollisionData getCollision(EntityID e1, EntityID e2){
            assert(didCollide(e1,e2));
            return std::make_pair(e1, e2);
        }
        CollisionData getCollision(entityName e1, EntityID e2){;
            assert(didCollide(e1,e2));
            return std::make_pair(nameMap.at(e1), e2);
        }
        CollisionData getCollision(EntityID e1, entityName e2){
            assert(didCollide(e1,e2));
            return std::make_pair(e1, nameMap.at(e2));
        }
        CollisionData getCollision(entityName e1, entityName e2){
            assert(didCollide(e1,e2));
            return std::make_pair(nameMap.at(e1), nameMap.at(e2));
        }

        inline bool didCollide(EntityID e1, EntityID e2){
            return Collided.contains(std::make_pair(e1, e2));
        }
        inline bool didCollide(entityName e1, EntityID e2){
            if(nameMap.contains(e1)){
                return Collided.contains(std::make_pair(nameMap.at(e1), e2));
            }
            return false;
        }
        inline bool didCollide(EntityID e1, entityName e2){
            if(nameMap.contains(e2)){
                bool res = Collided.contains(std::make_pair(nameMap.at(e2), e1));
                return res;
            }
            return false;
        }
        inline bool didCollide(entityName e1, entityName e2){
            if(nameMap.contains(e2) and nameMap.contains(e1)){
                return Collided.contains(std::make_pair(nameMap.at(e2), nameMap.at(e1)));
            }
            return false;
        }

        inline void setCollisionData(const std::set<std::pair<EntityID, EntityID>>& data){
            Collided = data;
        }
        inline void setNameMap(const std::unordered_map<entityName, EntityID>& nm){
            nameMap = nm;
        }
        inline void resetCollisionData(){
            Collided.clear();
        }
    private:
        std::set<std::pair<EntityID , EntityID>> Collided;
        std::unordered_map<entityName , EntityID> nameMap;
    };

    struct StatusLines{
        void set(size_t i, const std::string& s){
            renderSystem->setStatusLine(i, s);
        }
        void setRenderSystem(RenderSystem* rs){
            renderSystem = rs;
        }
    private:
        RenderSystem* renderSystem;
    };

    struct EntityData{
        using entityName = const char*;
        void destroy(EntityID id){
            entityManager->DestroyEntity(id);
        }
        void setMgr(EntityManager* mgr){
            entityManager = mgr;
        }
        EntityID get_ref(entityName name){
            return nameMap.at(name);
        }
        inline void setNameMap(const std::unordered_map<entityName, EntityID>& nm){
            nameMap = nm;
        }
    private:
        EntityManager* entityManager;
        std::unordered_map<entityName , EntityID> nameMap;
    };

private:
    virtual void end(){}
public:
    std::set<EntityID> Entities;
    Input Input;
    ComponentData ComponentData;
    DeltaTime DeltaTime;
    Collisions Collisions;
    Bounds Bounds;
    StatusLines StatusLines;
    EntityData EntityData;
    virtual bool update() {
        return true; // return false if game ends;
    }
    void end_base(){
        end();
    }
    virtual ~System_base() = default;
};

#endif //A4_SYSTEM_H
