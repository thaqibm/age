//
// Created by thaqi on 12/15/2021.
//

#ifndef A4_GAMECONTROLLER_H
#define A4_GAMECONTROLLER_H

#include <memory>
#include "AGE/view/render.h"
#include "AGE/core/AGEManager.h"

template<template<class...> class Seq, typename P, typename... Rest>
class GameController{
    using rendererType = std::unique_ptr<AGE::Seq2DRender<Seq, P, Rest...>>;
    rendererType W;

public:
    GameController(rendererType r) : W{std::move(r)} {}
    void run(AGEManager& mgr, size_t fps = 2, int exitCode = '0'){
        W->setState(mgr.getState());
        W->render();
        using State = std::vector<std::vector<char>>;
        W->runLoop([&mgr, &fps](State& s, int ch) -> State{
            mgr.update(1/(fps+0.0f), ch);
            s = mgr.getState();
            return s;
        }, exitCode);
    }
};



#endif //A4_GAMECONTROLLER_H
