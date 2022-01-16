//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_DATA_H
#define A4_DATA_H
#include <variant>
#include <vector>

struct Vec2d{
    float x;
    float y;
    Vec2d operator+(const Vec2d& other) const{
        return Vec2d{x+other.x, y+other.y};
    }
    Vec2d& operator+=(const Vec2d& other){
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2d& operator*(const float k){
        x *= k;
        y *= k;
        return *this;
    }
};

#endif //A4_DATA_H
