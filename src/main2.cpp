//
// Created by thaqi on 12/15/2021.
//

#include <vector>
#include <memory>
#include <iostream>

#include "GameController.h"
#include "AGE.h"
#include <random>
#include "Pong/art.h"





struct Score{
    int scoreA;
    int scoreB;
};

struct PlayerTag{
    bool playerAB;
};

struct ball_dir{
    int dir;
};




struct userInputSystem : System_base{
    bool update() override {

        for(const auto& et : Entities) {
            auto& pos = ComponentData.getComponentData<AGE_COMPONENTS::Position>(et);;
            auto& pt= ComponentData.getComponentData<PlayerTag>(et);
            if(Input.GetKey('w') and pt.playerAB){
                pos.p.y += 2;
            }
            if(Input.GetKey('s') and pt.playerAB){
                pos.p.y -= 2;
            }
            if(Input.GetKey('i') and !pt.playerAB){
                pos.p.y += 2;
            }
            if(Input.GetKey('k') and !pt.playerAB){
                pos.p.y -= 2;
            }


            auto& ball_pos = ComponentData.getComponentData<AGE_COMPONENTS::Position>(EntityData.get_ref("ball"));

            if(ball_pos.p.x >= Bounds.x || ball_pos.p.y >= Bounds.y || ball_pos.p.x <= 0 || ball_pos.p.y <= 0){
                ball_pos.p = {30,10};
                auto& vel = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(EntityData.get_ref("ball"));
                auto& dir = ComponentData.getComponentData<ball_dir>(EntityData.get_ref("ball"));
                vel = {(dir.dir*4+0.0f), 0};
                dir.dir *= -1;
            }

            if(Collisions.didCollide("player1", "ball")){
                auto& vel = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(EntityData.get_ref("ball"));
                std::vector<int> dir{-1,0,1};
                auto ri = dir[rand() % 3];
                if(ri == 0){
                    vel.v = {10,0};
                }
                else if(ri == 1){
                    vel.v = {7,2};
                }
                else{
                    vel.v = {7,-2};
                }
            }
            if(Collisions.didCollide("player2", "ball")){
                auto& vel = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(EntityData.get_ref("ball"));
                std::vector<int> dir{-1,0,1};
                auto ri = dir[rand() % 3];
                if(ri == 0){
                    vel.v = {-10,0};
                }
                else if(ri == 1){
                    vel.v = {-7,1};
                }
                else{
                    vel.v = {-7,-1};
                }
            }
        }
        return true;
    }
};

using namespace std;
int main(int argc, char** argv)
{


    auto makePlayer = [](bool A, float x, float y) -> auto{
        return Prefab<AGE_COMPONENTS::Position, AGE_COMPONENTS::Drawable, AGE_COMPONENTS::Velocity, AGE_COMPONENTS::Acceleration, AGE_COMPONENTS::BoxCollider, AGE_COMPONENTS::Solid_tag, PlayerTag>
                (
                "player",
                {{x,y},0, 4, 1},
                {{{'#'}, {'#'}, {'#'}, {'#'}}},
                {{0,0}},
                {{0,0}},
                {4,2},
                {},
                {A}
        );
    };


    Prefab<AGE_COMPONENTS::Position, AGE_COMPONENTS::Drawable, AGE_COMPONENTS::Velocity, AGE_COMPONENTS::Acceleration, AGE_COMPONENTS::BoxCollider, ball_dir>
            ball(
            "ball",
            {{30,10},0},
            {{{'o'}}},
            {{-4,0}},
            {{0,0}},
            {1,1},
            {1}
    );



    AGEManager GAME1{};
    AGE_SYSTEMS::addPhysicsSystem(GAME1);
    GAME1.addComponent<PlayerTag>();
    GAME1.addComponent<ball_dir>();
    GAME1.registerSystem<userInputSystem>();
    GAME1.SetSystemTraits<userInputSystem, AGE_COMPONENTS::Position,PlayerTag>();



    auto p1 = GAME1.createEntityFromPrefab(makePlayer(true, 3, 1), "player1");

    auto p2 = GAME1.createEntityFromPrefab(makePlayer(false, 72, 2), "player2");

    auto be = GAME1.createEntityFromPrefab(ball, "ball");

    int fps = (argc <= 1)  ? 2 : stoi(argv[1]);
    GameController<vector, char> g{make_unique<AGE::CursesRender>(25, 80, fps,make_shared<AGE::CursesWindow>("AGE Engine v1.0 (0 to Exit"))} ;

    g.run(GAME1, fps, '0');

    return 0;
}
