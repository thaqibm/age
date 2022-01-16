//
// Created by thaqi on 12/13/2021.
//

#ifndef A4_PREFAB_H
#define A4_PREFAB_H

#include <tuple>
#include "util/unique_assert.h"

template<typename... Components>
struct Prefab{
    static_assert(is_unique<Components...>, "Components should be unique");
    const char* name;
    std::tuple<Components...> data;
public:
    Prefab(const char* name, Components&&... components): name{name}, data(components...)
    {
    }
    [[nodiscard]] size_t size() const{
        return data.size();
    }


};




#endif //A4_PREFAB_H
