//
// Created by thaqi on 12/15/2021.
//

#ifndef A4_QUADRATICSOLVER_H
#define A4_QUADRATICSOLVER_H

#include <utility>
#include <cmath>
#include <cassert>

std::pair<float, float> solveQuadratic(float A, float B, float C){
    float D = B*B - 4*A*C;
    if(D< 0){
        return {0,0};
    }
    float y1 = (-1*B + std::sqrt(D))/(2*A);
    float y2 = (-1*B - std::sqrt(D))/(2*A);
    return {y1, y2};
}



#endif //A4_QUADRATICSOLVER_H
