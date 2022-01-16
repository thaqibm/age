//
// Created by thaqi on 12/13/2021.
//

#ifndef A4_NTHTYPE_H
#define A4_NTHTYPE_H
#include <tuple>

/**
 * Get n-th element from varradiac template.
 * Code taken from:
 * https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
 */
template<int N, typename... Ts> using NthTypeOf =
typename std::tuple_element<N, std::tuple<Ts...>>::type;
#endif //A4_NTHTYPE_H
