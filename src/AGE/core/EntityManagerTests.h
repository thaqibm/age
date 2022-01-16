//
// Created by thaqi on 12/8/2021.
//

#ifndef A4_ENTITYMANAGERTESTS_H
#define A4_ENTITYMANAGERTESTS_H
#include "EntityManager.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <functional>

namespace TEST {
    std::vector<std::function<void()>> tests{
            []() -> void {
                using namespace std;
                EntityManager age{};
                age.CreateEntity();
                auto ett = age.CreateEntity();
                assert(age.size() == 2 && "Create 2 entt");
                age.DestroyEntity(ett);
                assert(age.size() == 1 && "Destroyed 1 entt");
                cout << "test0 output: "<<age.getSignature(age.CreateEntity()) << endl;
            }
    };

    void runTests(){
        for(const auto& t : tests){
            t();
        }
    }
    void runTesti(size_t i){
        assert(i < tests.size());
        tests[i]();
    }

}

#endif //A4_ENTITYMANAGERTESTS_H
