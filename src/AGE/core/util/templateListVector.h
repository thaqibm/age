//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_TEMPLATELISTVECTOR_H
#define A4_TEMPLATELISTVECTOR_H
#include <vector>

template<typename Last>
std::vector<const char*> type_vector(std::vector<const char*>& acc){
    acc.emplace_back(typeid(Last).name());
}
template<typename First, typename Second, typename ...Rest>
std::vector<const char*> type_vector(std::vector<const char*>& acc) {
    acc.template emplace_back(typeid(First).name());
    return type_vector<Second, Rest...>(acc);
}

template<typename... TYPES>
std::vector<const char*> makeTypeNameVector(){
    std::vector<const char*> v{};
    return type_vector<TYPES...>(v);
}


#endif //A4_TEMPLATELISTVECTOR_H
