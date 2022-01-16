//
// Created by thaqi on 12/9/2021.
//

#include <cmath>
#include <unordered_map>
#include <utility>
#include "RenderSystem.h"
#include "components/default/default.h"

void RenderSystem::init() {
    for(size_t i = 0; i<state.size()-nStatus-1; ++i){
        state[i][0] = '|';
        state[i][state[0].size()-1] = '|';
    }
    for(size_t i = 0; i<state[0].size(); ++i){
        state[0][i] = '-';
        state[state.size()-nStatus-1][i] = '-';
    }
    state[state.size()-nStatus][0] = '1';
    state[state.size()-nStatus+1][0] = '2';
    state[state.size()-nStatus+2][0] = '3';
    state[state.size()-nStatus-1][0] = '+';
    state[state.size()-nStatus-1][state[0].size()-1] = '+';
    state[0][state[0].size()-1] = '+';
    state[0][0] = '+';

    ctx->registerComponent<AGE_COMPONENTS::Position>();
    ctx->registerComponent<AGE_COMPONENTS::Drawable>();
    ctx->registerComponent<AGE_COMPONENTS::Solid_tag>();
}
void RenderSystem::setStatusLine(size_t n, const std::string &s) {
    assert(n > 0 and n <= nStatus);
    for(size_t i = 0; i<std::min(s.size(), state[0].size()); ++i){
        state[state.size() - nStatus + n - 1][i] = s[i];
    }
}
void RenderSystem::renderDrawable(){


    ComponentSignature render_signature;
    render_signature.set(ctx->getComponentId<AGE_COMPONENTS::Position>(), true);
    render_signature.set(ctx->getComponentId<AGE_COMPONENTS::Drawable>(), true);

    ComponentSignature solid_signature;
    solid_signature.set(ctx->getComponentId<AGE_COMPONENTS::Solid_tag>(), true);


    clearState();

    auto allEntities = entityManager->getEntities();

    for(auto e : allEntities){
        auto [isAlive, signature] = entityManager->getEntity(e);
        if((solid_signature & signature) == solid_signature){
            auto& p = ctx->getComponentData<AGE_COMPONENTS::Position>(e);
            auto pos_x = clamp(p.p.x, state[0].size() - p.width - 2, 0.0f);
            auto pos_y = clamp(p.p.y, state.size() - 2 -nStatus -p.len, 0.0f);
            p.p.x = pos_x;
            p.p.y = pos_y;
        }
        if(isAlive and (render_signature & signature)==render_signature){
            auto& [p, h,l, w] = ctx->getComponentData<AGE_COMPONENTS::Position>(e);
            auto& [x,y] = p;
            auto& drawData = ctx->getComponentData<AGE_COMPONENTS::Drawable>(e).data;
            drawXY(std::ceil(x), std::ceil(y), std::ceil(h), drawData);
        }
    }
}

void RenderSystem::clearState() {
    for(size_t row = 1; row<state.size()-nStatus-1; ++row){
        for(size_t col = 1; col<state[0].size()-1; ++col){
            state[row][col] = ' ';
        }
    }
}

void RenderSystem::drawXY(int x0, int y0, int h,const std::vector<std::vector<char>>& v){

    const int n = v.size();
    for(int y = 0; y<n; ++y){
        for(int x = 0; x<v[y].size(); ++x){
            setStateXY(x0+x, y0+n-y-1, v[y][x]);
        }
    }
}


void RenderSystem::setStateXY(int x, int y, char c){
    int x1 = x+1;
    int y1 = state.size() - nStatus - 2 -y;
    if(x1 >= 1 and y1 >= 1 and x1 <= state[0].size() - 2 and y1 <= state.size() - nStatus - 2){
        state[y1][x1] = c;
    }
}
