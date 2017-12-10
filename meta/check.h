#pragma once
#include <tuple>

namespace meta {

template<class... A>
constexpr bool check() {
    return std::tuple<A...>::fail();
}

} // namespace meta
