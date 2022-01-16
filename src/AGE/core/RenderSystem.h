//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_RENDERSYSTEM_H
#define A4_RENDERSYSTEM_H

#include <vector>
#include <string>
#include <map>
#include <utility>
#include "components/componentManager.h"
#include "EntityManager.h"
class RenderSystem {
public:
    using State = std::vector<std::vector<char>>;
private:
    State state;
    componentManager* ctx;
    EntityManager* entityManager;
    size_t nStatus;
public:
    RenderSystem(size_t row, size_t col,componentManager* ctx, EntityManager* entityManager,size_t nStatus=3):
    state{row, std::vector<char>(col, ' ')},
    ctx{ctx},
    entityManager{entityManager},
    nStatus{nStatus}
    {
    }
    void init();
    void renderDrawable();
    inline const State& getState(){
        return state;
    }
    void setStatusLine(size_t n, const std::string& s);

private:
    void drawXY(int x0, int y0, int h,const std::vector<std::vector<char>>& v);
    void setStateXY(int x, int y, char c);
    void clearState();
    template<class T>
    const T& clamp(const T& x, const T& upper, const T& lower) {
        return std::min(upper, std::max(x, lower));
    }
};


#endif //A4_RENDERSYSTEM_H
