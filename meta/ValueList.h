#pragma once
#include "Value.h"

#include <algorithm>
#include <cinttypes>
#include <utility>

namespace meta {

namespace details {

template<auto V, auto... W>
constexpr auto head() noexcept -> decltype(V) {
    return {};
}

template<auto... W>
constexpr bool isSameType() noexcept {
    if constexpr (sizeof...(W) >= 1) //
        return (true && ... && std::is_same_v<decltype(head<W...>()), decltype(W)>);
    return false;
}

template<auto... W>
constexpr auto commonType() noexcept {
    if constexpr (isSameType<W...>()) return head<W...>();
}

template<class T>
constexpr auto underlyingType() noexcept {
    if constexpr (std::is_enum_v<T>) {
        return std::underlying_type_t<T>{};
    }
    else {
        return std::declval<T>();
    }
}

template<auto T, auto... A, size_t... I>
constexpr auto indexOf(std::index_sequence<I...> = {}) noexcept -> size_t {
    constexpr auto max = std::numeric_limits<size_t>::max();
    return std::min({max, (Value<T>{} == Value<A>{} ? I : max)...});
}

template<auto... A, size_t... I>
constexpr auto isValueSet(std::index_sequence<I...> s = {}) noexcept -> bool {
    return (true && ... && (I == indexOf<A, A...>(s)));
}

template<auto A, bool C>
struct ValueFold {
    template<auto B, bool D>
    constexpr auto operator||(ValueFold<B, D> o) const noexcept {
        if constexpr (C)
            return *this;
        else
            return o;
    }
    constexpr auto value() const noexcept -> Value<A> { return {}; }
};

template<size_t Index, auto... A, size_t... I>
constexpr auto atIndex(std::index_sequence<I...> = {}) noexcept {
    return (ValueFold<A, Index == I>{} || ... || ValueFold<nullptr, false>{}).value();
}

template<auto... A>
constexpr auto min() noexcept {
    if constexpr (isSameType<A...>())
        return std::min({A...});
    else
        return nullptr;
}

template<auto... A>
constexpr auto max() noexcept {
    if constexpr (isSameType<A...>())
        return std::max({A...});
    else
        return nullptr;
}

template<auto... A>
struct Values {
    template<template<auto...> class N>
    constexpr static auto to() {
        return N<A...>{};
    }
};

template<bool C, auto A>
struct ConditionalValue {
    template<auto... B>
    constexpr auto operator+=(Values<B...> o) const noexcept {
        if constexpr (C)
            return Values<A, B...>{};
        else
            return o;
    }
};

template<auto B, auto... A>
constexpr auto filter() noexcept {
    return (ConditionalValue<Value<A>{} != Value<B>{}, A>{} += ... += Values<>{});
}

} // namespace details

template<auto... V>
struct ValueList {
    static constexpr bool isSameType = details::isSameType<V...>();
    using commonType = decltype(details::commonType<V...>());
    using underlyingType = decltype(details::underlyingType<commonType>());

    using IndexSequence = std::make_index_sequence<sizeof...(V)>;
    static constexpr auto indexSequence = IndexSequence{};

    static constexpr auto min = details::min<V...>();
    static constexpr auto max = details::max<V...>();
    static constexpr bool isSet = details::isValueSet<V...>(indexSequence);

    template<auto B>
    static constexpr auto indexOf(Value<B> = {}) noexcept -> size_t {
        return details::indexOf<B, V...>(indexSequence);
    }

    template<size_t I>
    static constexpr auto atIndex(Value<I> = {}) noexcept {
        return details::atIndex<I, V...>(indexSequence);
    }

    template<auto B>
    static constexpr auto filter(Value<B> = {}) noexcept {
        return details::filter<B, V...>().template to<ValueList>();
    }
    template<class F>
    static constexpr void each(F &&f) noexcept {
        (f(Value<V>{}), ...);
    }
    template<template<auto...> class T>
    static constexpr auto to() noexcept -> T<V...> {
        return {};
    }
};

template<auto... A, auto... B>
constexpr auto operator+(ValueList<A...>, ValueList<B...>) -> ValueList<A..., B...> {
    return {};
}

static_assert(ValueList<1, 2, 3>::indexOf<1>() == 0, "indexOf does not work");
static_assert(ValueList<1, 2, 3>::indexOf<2>() == 1, "indexOf does not work");
static_assert(ValueList<1, 2, 3>::indexOf<3>() == 2, "indexOf does not work");
static_assert(ValueList<0, 2, 3>::indexOf<1>() == std::numeric_limits<size_t>::max(), "indexOf does not work");
static_assert(ValueList<>::indexOf<1>() == std::numeric_limits<size_t>::max(), "indexOf does not work");

static_assert(ValueList<1, 2, 3>::isSameType, "sameType does not work");
static_assert(!ValueList<1, 'a', 3>::isSameType, "sameType does not work");

static_assert(std::is_same_v<ValueList<1, 2, 3>::commonType, int>, "commonType does not work");
static_assert(std::is_same_v<ValueList<1, '2', 3>::commonType, void>, "commonType does not work");

static_assert(ValueList<1, 2, 3>::min == 1, "min does not work");
static_assert(ValueList<1, 2, 3>::max == 3, "max does not work");

static_assert(ValueList<1, 2, 3>::isSet, "isSet does not work");
static_assert(!ValueList<1, 2, 2>::isSet, "isSet does not work");

static_assert(std::is_same_v<decltype(ValueList<1, 2, 3, 2>::filter<2>()), ValueList<1, 3>>, "filter does not work");

} // namespace meta
