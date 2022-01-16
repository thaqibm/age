//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_DEFAULT_H
#define A4_DEFAULT_H

#include "data.h"
#include <memory>
#include <utility>
#include <functional>

namespace AGE_COMPONENTS{


    struct Position{
        Vec2d p;
        float z; // height
        float len;
        float width;
    };
    struct Velocity{
        Vec2d v;
    };
    struct Acceleration{
        Vec2d a;
    };

    struct BoxCollider{
        float width;
        float height;
    };

    struct CircleCollider{
        float radius;
    };

    struct Drawable{
        std::vector<std::vector<char>> data;
    };


    struct dynamicPosition{
        using funcType = std::function<Vec2d(Vec2d, float)>;
        funcType pt;
    };

    struct Mass{
        float mass;
    };


    struct Solid_tag{};




}


#endif //A4_DEFAULT_H
