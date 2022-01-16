//
// Created by thaqi on 12/13/2021.
//

#ifndef A4_UNIQUE_ASSERT_H
#define A4_UNIQUE_ASSERT_H


#include <type_traits>
/**
 * Check if template list is unique:
 * The following code was taken from:
 * https://stackoverflow.com/questions/47511415/checking-if-variadic-template-parameters-are-unique-using-fold-expressions
 * */

template <typename...>
inline constexpr auto is_unique = std::true_type{};

template <typename T, typename... Rest>
inline constexpr auto is_unique<T, Rest...> = std::__bool_constant<
                                              (!std::is_same<T, Rest>::value && ...) && is_unique<Rest...>
                                                                                        >{};
#endif //A4_UNIQUE_ASSERT_H
