#pragma once
#include "type_traits"

namespace meta {

// invalid prototype is required for HasName to compile
constexpr auto name() -> void;

template<class, class = void>
struct HasName : std::false_type {};

template<class T>
struct HasName<T, std::void_t<decltype(name(std::declval<T>()))>> : std::true_type {};

template<class T>
constexpr auto hasName = HasName<T>::value;

} // namespace meta
