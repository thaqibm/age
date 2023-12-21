#include <vector>
#include <memory>
#include <iostream>

#include "AGE.h"
#include "SpaceInvaders/art.h"
#include "GameController.h"

template<template<class...> class Seq, typename P, typename... Rest>
struct G{
	std::unique_ptr<AGE::Seq2DRender<Seq, P, Rest...>> W;
};



struct Score{
    int score;
};


struct PlayerTag{
    EntityID bullet;
    int current_dir;
};

struct levelSystem : System_base{
    bool update() override{
        for(const auto& et : Entities){
            auto score_ett = EntityData.get_ref("score");
            auto& [score] = ComponentData.getComponentData<Score>(score_ett);
            auto curr_level = score/20;
            StatusLines.set(3, "Level " + std::to_string(curr_level));

            std::vector<char> gg;
            for(int i = 0; i<curr_level; ++i){
                gg.push_back('g');
            }
            std::vector<std::vector<char>> v{gg};

            auto& goal_keeper = ComponentData.getComponentData<AGE_COMPONENTS::Drawable>(et);
            goal_keeper.data = v;
            auto& goal_keeper_collider = ComponentData.getComponentData<AGE_COMPONENTS::BoxCollider>(et);
            goal_keeper_collider.width = curr_level;
            goal_keeper_collider.height = std::max(curr_level, 1);
        }

        return true;
    }
};

struct userShooterSystem : System_base{
    bool update() override {

        for(const auto& et : Entities) {
            auto& pos = ComponentData.getComponentData<AGE_COMPONENTS::Position>(et);;
            auto& v = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(et);;
            auto& pt= ComponentData.getComponentData<PlayerTag>(et);
            auto bullet = pt.bullet;
            if(Input.GetKey('w')){
                pos.p.y += 1;
                v.v.y += 2;
            }
            if(Input.GetKey('s')){
                pos.p.y -= 1;
                v.v.y -= 1;
            }
            if(Input.GetKey('a')){
                pos.p.x -= 2;
                v.v.x -= 1;
            }
            if(Input.GetKey('d')){
                pos.p.x += 2;
                v.v.x += 1;
            }
            if(Input.GetKey(' ')){ // shoot
                auto& p0 = ComponentData.getComponentData<AGE_COMPONENTS::Position>(bullet);
                auto [p, h, l, w] = p0;
                if(p.y >= 0 and p.y<=20 and p.x >= 0 and p.x<= 75){

                }
                else{
                    p0.p = pos.p + Vec2d{0,4};
                    auto& v0 = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(bullet);
                    v0 = {pt.current_dir*10 + 0.0f,10};
                }
            }
            if(Input.GetKey('e')){
                auto& d = ComponentData.getComponentData<AGE_COMPONENTS::Drawable>(et);
                d.data[0][2] = '/';
                d.data[0][1] = ' ';
                d.data[0][0] = ' ';
                pt.current_dir = 1;
            }
            if(Input.GetKey('q')){
                auto& d = ComponentData.getComponentData<AGE_COMPONENTS::Drawable>(et);
                d.data[0][2] = ' ';
                d.data[0][1] = ' ';
                d.data[0][0] = '\\';
                pt.current_dir = -1;
            }
            if(Input.GetKey('2')){
                auto& d = ComponentData.getComponentData<AGE_COMPONENTS::Drawable>(et);
                d.data[0][2] = ' ';
                d.data[0][1] = '|';
                d.data[0][0] = ' ';
                pt.current_dir = 0;
            }

            auto score_ett = EntityData.get_ref("score");
            auto& score = ComponentData.getComponentData<Score>(score_ett);

            if(Collisions.didCollide(bullet, "goal")){
                score.score += 5;
                StatusLines.set(1, "Score: " + std::to_string(score.score));
                StatusLines.set(2, "Goal Scored! +5" );
                auto& p0 = ComponentData.getComponentData<AGE_COMPONENTS::Position>(bullet);
                p0.p.y = -1;
                p0.p.x = -1;
            }
            if(Collisions.didCollide(bullet, "keeper")){
                score.score -= 1;
                StatusLines.set(1, "Score: " + std::to_string(score.score));
            }
        }
        return true;
    }
};


struct GAME1Collider : System_base{
    bool update() override {
        if(Collisions.didCollide("bullet", "keeper")){
            StatusLines.set(2, "Goal Saved! -1");
            auto& v = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(EntityData.get_ref("bullet"));
            v.v = {-10,-10};
        }
    }
};

using namespace std;
int main(int argc, char** argv)
{


    int fps = (argc <= 1)  ? 2 : stoi(argv[1]);
    int h = (argc <= 2) ? 0 : stoi(argv[2]);
    Prefab<AGE_COMPONENTS::Position, AGE_COMPONENTS::Drawable, AGE_COMPONENTS::Velocity, AGE_COMPONENTS::Acceleration, AGE_COMPONENTS::BoxCollider, AGE_COMPONENTS::Mass> bullet(
            "bullet",
            {{-1,-1},0, 0,0},
            {{{'o'}}},
            {{0,0}},
            {{0,0}},
            {1,1},
            {1.0f}
    );


    AGEManager GAME1{};
    AGE_SYSTEMS::addPhysicsSystem(GAME1);
    AGE_SYSTEMS::addDynamicPhysics(GAME1);


    struct enemy_tag{
        int m;
    };
    /** user defined **/
    GAME1.addComponent<PlayerTag>();
    GAME1.addComponent<AGE_COMPONENTS::Mass>();
    GAME1.addComponent<enemy_tag>();
    GAME1.addComponent<Score>();
    GAME1.registerSystem<userShooterSystem>();
    GAME1.registerSystem<levelSystem>();
    GAME1.registerSystem<GAME1Collider>();
    GAME1.SetSystemTraits<userShooterSystem, AGE_COMPONENTS::Position, AGE_COMPONENTS::Velocity,PlayerTag, AGE_COMPONENTS::Drawable>();
    GAME1.SetSystemTraits<levelSystem, enemy_tag, AGE_COMPONENTS::dynamicPosition>();



    Prefab<AGE_COMPONENTS::Drawable, AGE_COMPONENTS::Position, AGE_COMPONENTS::BoxCollider, AGE_COMPONENTS::Mass, AGE_COMPONENTS::Velocity, AGE_COMPONENTS::dynamicPosition, enemy_tag>
            ex_prefab(
                    "goal_keeper",
                    {{{'g'}}},
                    {{40, 5}, 0, 2, 3},
                    {1,1},
                    {5000.0f},
                    {1,1},
                    {[](Vec2d p0, float t) -> Vec2d{
                        return {static_cast<float>(35 + 8*cos(0.7*(t))), 9};
                    }},
                    {1}
            );

    Prefab<AGE_COMPONENTS::Drawable, AGE_COMPONENTS::Position, AGE_COMPONENTS::BoxCollider>
            goal_post(
                    "goal",
                    {goal},
                    {{30, 15}, static_cast<float>(h), 0,0},
                    {5, 8}
                    );



    auto er = GAME1.createDrawableEntity({{0,0},1, 2, 3},
                                         {{{' ', '|', ' '}, {' ', 'O', ' '}} });


    GAME1.createEntityFromPrefab(ex_prefab, "keeper");
    auto _et = GAME1.createEntityFromPrefab(goal_post, "goal");


    GAME1.attachComponent<Score>(GAME1.createEntity("score"), {0});






    GAME1.attachComponent<PlayerTag>(er, {GAME1.createEntityFromPrefab(bullet, "bullet"), 0});
    GAME1.attachComponent<AGE_COMPONENTS::Velocity>(er, {{0,0}});
    GAME1.attachComponent<AGE_COMPONENTS::Solid_tag>(er, {});

    GAME1.setStatusLine(1, "Score: 0");




    //auto dw = make_shared<AGE::CursesWindow>("AGE Engine v1.0 (0 to exit)");
    //G<vector, char>  g{make_unique<AGE::CursesRender>(25,80, fps, dw)};

    GameController<vector, char> g{make_unique<AGE::CursesRender>(25, 80, fps,make_shared<AGE::CursesWindow>("AGE Engine v1.0 (q to Exit"))} ;

    g.run(GAME1, fps, '0');
    return 0;
}
