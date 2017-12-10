#pragma once
#include "Type.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>

namespace meta {

namespace details {

template<class V, class... W>
constexpr auto head() noexcept -> Type<V> {
    return {};
}

template<class T, class... A, size_t... I>
constexpr auto indexOf(std::index_sequence<I...> = {}) noexcept -> size_t {
    constexpr auto max = std::numeric_limits<size_t>::max();
    return std::min({max, (Type<T>{} == Type<A>{} ? I : max)...});
}

template<class... A, size_t... I>
constexpr auto isTypeSet(std::index_sequence<I...> s = {}) noexcept -> bool {
    return (true && ... && (I == indexOf<A, A...>(s)));
}

template<class... A>
struct Types {
    template<template<class...> class N>
    constexpr static auto to() {
        return N<A...>{};
    }
};

template<bool C, class A>
struct ConditionalType {
    template<class... B>
    constexpr auto operator+=(Types<B...> o) const noexcept {
        if constexpr (C)
            return Types<A, B...>{};
        else
            return o;
    }
};

template<class B, class... A>
constexpr auto filter() noexcept {
    return (ConditionalType<Type<A>{} != Type<B>{}, A>{} += ... += Types<>{});
}

} // namespace details

template<class... A>
struct TypeList {
    using IndexSequence = std::index_sequence_for<A...>;
    static constexpr auto indexSequence = IndexSequence{};

    template<class B>
    static constexpr auto indexOf(Type<B> = {}) noexcept -> size_t {
        return details::indexOf<B, A...>(indexSequence);
    }
    template<class B>
    static constexpr auto filter(Type<B> = {}) noexcept {
        return details::filter<B, A...>().template to<TypeList>();
    }
    static constexpr auto isSet = details::isTypeSet<A...>(indexSequence);

    template<class F>
    static constexpr void each(F &&f) noexcept {
        (f(Type<A>{}), ...);
    }
    template<template<class...> class B>
    static constexpr auto to() noexcept -> B<A...> {
        return {};
    }
};

template<class... A, class... B>
constexpr auto operator+(TypeList<A...>, TypeList<B...>) -> TypeList<A..., B...> {
    return {};
}

template<class T, class... A>
constexpr auto indexOf(TypeList<A...> a = {}, Type<T> = {}) noexcept -> size_t {
    return details::indexOf<T, A...>(a.indexSequence);
}

template<class T, class... A>
constexpr auto filter(TypeList<A...> = {}, Type<T> = {}) noexcept {
    return details::filter<T, A...>().template to<TypeList>();
}

static_assert(TypeList<int, char, float>::indexOf<int>() == 0, "");
static_assert(TypeList<char, int, float>::indexOf<int>() == 1, "");
static_assert(TypeList<char, float, int>::indexOf<int>() == 2, "");
static_assert(TypeList<char, float>::indexOf<int>() == std::numeric_limits<size_t>::max(), "");
static_assert(TypeList<>::indexOf<int>() == std::numeric_limits<size_t>::max(), "");

static_assert(TypeList<int, char, float>::isSet == true, "isSet failed");
static_assert(TypeList<int, char, int>::isSet == false, "isSet failed");

static_assert(std::is_same_v<decltype(TypeList<int, char, float, int>::filter<int>()), TypeList<char, float>>,
              "filter failed");

} // namespace meta
