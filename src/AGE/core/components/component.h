//
// Created by thaqi on 12/8/2021.
//

#ifndef A4_COMPONENT_H
#define A4_COMPONENT_H

#include <cstdint>
#include <bitset>

using ComponentID = std::uint16_t;
static const constexpr ComponentID Max_Components = 64;
using ComponentSignature = std::bitset<Max_Components>;


#endif //A4_COMPONENT_H
