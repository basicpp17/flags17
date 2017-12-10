#pragma once
#include <type_traits>

namespace meta {

template<auto V>
struct Value {
    template<template<auto> class T>
    constexpr static auto to() -> T<V> {
        return {};
    }
    constexpr static auto value() -> decltype(V) { return V; }
};

template<auto A, auto B>
constexpr bool operator==(Value<A>, Value<B>) noexcept {
    if constexpr (std::is_same_v<decltype(A), decltype(B)>)
        return std::bool_constant<A == B>{};
    else
        return std::false_type{};
}

template<auto A, auto B>
constexpr bool operator!=(Value<A> a, Value<B> b) noexcept {
    return !(a == b);
}

} // namespace meta
