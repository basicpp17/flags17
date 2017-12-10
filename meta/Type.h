#pragma once
#include <type_traits>

namespace meta {

template<class T>
struct Type {
    template<template<class> class B>
    constexpr static auto to() -> B<T> {
        return {};
    }
};

template<class A, class B>
constexpr bool operator==(Type<A>, Type<B>) noexcept {
    if constexpr (std::is_same_v<A, B>)
        return std::true_type{};
    else
        return std::false_type{};
}

template<class A, class B>
constexpr bool operator!=(Type<A> a, Type<B> b) noexcept {
    return !(a == b);
}

} // namespace meta
