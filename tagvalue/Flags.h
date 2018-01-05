#pragma once
#include "meta/Value.h"
#include "meta/ValueList.h"

#include "meta/details/BitStorage.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace tagvalue {

template<auto A>
struct Flag {};

template<auto... A>
struct FlagList {};

template<auto... A>
struct Flags {
    using This = Flags;
    using AllOptions = meta::ValueList<A...>;
    using FilteredOptions = decltype(AllOptions::template filter<nullptr>());
    static_assert(FilteredOptions::isSet, "do not repeat values");

    constexpr static auto options = FilteredOptions{};
    constexpr static auto bitCount = sizeof...(A);

    template<auto B>
    constexpr static auto indexOf(Flag<B> = {}) noexcept {
        static_assert(!std::is_same_v<decltype(B), std::nullptr_t>, "nullptr is not a valid tag");
        return AllOptions::template indexOf<B>();
    }

    template<auto B, auto... C>
    constexpr Flags(Flag<B>, Flag<C>...) noexcept
        : storage(std::index_sequence<indexOf<B>(), indexOf<C>()...>{}) {
        static_assert(((bitCount > indexOf<B>()) && ... && (bitCount > indexOf<C>())));
    }

    template<auto... B>
    constexpr Flags(FlagList<B...> = {}) noexcept
        : storage(std::index_sequence<indexOf<B>()...>{}) {
        static_assert((true && ... && (bitCount > indexOf<B>())));
    }

    constexpr Flags() noexcept = default;
    ~Flags() noexcept = default;
    constexpr Flags(const This &) noexcept = default;
    constexpr Flags(This &&) noexcept = default;
    constexpr auto operator=(const This &) noexcept -> This & = default;
    constexpr auto operator=(This &&) noexcept -> This & = default;

    constexpr bool operator==(const This &o) const noexcept { return storage == o.storage; }
    constexpr bool operator!=(const This &o) const noexcept { return !(*this == o); }

    template<auto B>
    constexpr bool operator[](Flag<B>) const noexcept {
        constexpr auto index = indexOf<B>();
        static_assert(bitCount > index);
        return storage[index];
    }

    constexpr bool all() const noexcept { return all(setAll()); }
    constexpr bool all(This b) const noexcept { return (storage & b.storage) == b.storage; }
    template<auto B, auto... C>
    constexpr bool all(FlagList<B, C...> b = {}) const noexcept {
        static_assert(((bitCount > indexOf<B>()) && ... && (bitCount > indexOf<C>())));
        return all(This{b});
    }
    template<auto B, auto... C>
    constexpr bool all(Flag<B>, Flag<C>...) const noexcept {
        return all<B, C...>();
    }

    constexpr bool any() const noexcept { return any(setAll()); }
    constexpr bool any(This b) const noexcept { return (storage & b.storage) != Storage{}; }
    template<auto B, auto... C>
    constexpr bool any(FlagList<B, C...> b = {}) const noexcept {
        static_assert(((bitCount > indexOf<B>()) && ... && (bitCount > indexOf<C>())));
        return any(This{b});
    }
    template<auto B, auto... C>
    constexpr bool any(Flag<B>, Flag<C>...) const noexcept {
        return any<B, C...>();
    }

    constexpr bool none() const noexcept { return any(setAll()); }
    constexpr bool none(This b) const noexcept { return (storage & b.storage) == Storage{}; }
    template<auto B, auto... C>
    constexpr bool none(FlagList<B, C...> b = {}) const noexcept {
        static_assert(((bitCount > indexOf<B>()) && ... && (bitCount > indexOf<C>())));
        return none(This{b});
    }
    template<auto B, auto... C>
    constexpr bool none(Flag<B>, Flag<C>...) const noexcept {
        return none<B, C...>();
    }

    constexpr static auto setAll() noexcept -> This { return This{options.template to<FlagList>()}; }
    constexpr static auto resetAll() noexcept -> This { return This{}; }
    constexpr auto flipAll() const noexcept -> This {
        auto r = This{};
        r.storage = storage ^ setAll().storage;
        return r;
    }

    constexpr auto set(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage | b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto set(FlagList<B...> b = {}) const noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf<B>())));
        return set(This{b});
    }
    template<auto B, auto... C>
    constexpr auto set(Flag<B>, Flag<C>...) const noexcept -> This {
        return set<B, C...>();
    }

    constexpr auto reset(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage & ~b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto reset(FlagList<B...> b = {}) const noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf<B>())));
        return reset(This{b});
    }
    template<auto B, auto... C>
    constexpr auto reset(Flag<B>, Flag<C>...) const noexcept -> This {
        reset<B, C...>();
    }

    constexpr auto flip(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage ^ b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto flip(FlagList<B...> b = {}) noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf<B>())));
        return flip(This{b});
    }
    template<auto B, auto... C>
    constexpr auto flip(Flag<B>, Flag<C>...) noexcept -> This {
        return flip<B, C...>();
    }

    constexpr auto mask(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage & b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto mask(FlagList<B...> b = {}) noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf<B>())));
        return mask(This{b});
    }
    template<auto B, auto... C>
    constexpr auto mask(Flag<B>, Flag<C>...) noexcept -> This {
        return mask<B, C...>();
    }

    constexpr auto operator~() const noexcept -> This { return flipAll(); }

    template<class B>
    constexpr auto operator|(B b) const noexcept -> This {
        return set(b);
    }
    template<class B>
    constexpr auto operator&(B b) const noexcept -> This {
        return mask(b);
    }
    template<class B>
    constexpr auto operator^(B b) const noexcept -> This {
        return flip(b);
    }

    template<class B>
    constexpr auto operator|=(B b) noexcept -> This & {
        return *this = set(b);
    }
    template<class B>
    constexpr auto operator&=(B b) noexcept -> This & {
        return *this = mask(b);
    }
    template<class B>
    constexpr auto operator^=(B b) noexcept -> This & {
        return *this = flip(b);
    }

    template<class F>
    constexpr void each(F &&f) const noexcept {
        options.each([&, ff = std::forward<F>(f)](auto value) mutable {
            auto flag = value.template to<Flag>();
            ff(flag, (*this)[flag]);
        });
    }

    template<class F>
    constexpr void each_set(F &&f) const noexcept {
        each([ff = std::forward<F>(f)](auto flag, bool s) mutable {
            if (s) ff(flag);
        });
    }

    template<class F>
    constexpr void each_reset(F &&f) const noexcept {
        each([ff = std::forward<F>(f)](auto flag, bool s) mutable {
            if (!s) ff(flag);
        });
    }

private:
    using Storage = decltype(meta::details::SelectBitStorage<bitCount>());
    Storage storage{};
};

static_assert(Flags<1, 2, 3>{FlagList<1, 2>{}} == FlagList<1, 2>{}, "");
static_assert((Flags<1, 2>{} | Flag<1>{}) == Flag<1>{}, "");
static_assert((Flags<1, 2, 3>{}.set<1, 3>() & FlagList<1, 2>{}) == Flag<1>{}, "");
static_assert(Flags<1, 2, 3>{}.flip<2, 3>().any(Flag<1>{}, Flag<2>{}), "");
static_assert((Flags<1, 2, 3>::setAll() & Flag<2>{}) == Flag<2>{}, "not all set");

template<class T>
struct Typed {
    T value;

    Typed(T v)
        : value(v) {}
};
template<class, auto, class = void>
struct HasTyped : std::false_type {};

template<class Out, auto A>
struct HasTyped<Out, A, std::void_t<decltype(operator<<(std::declval<Out>(), Typed{A}))>> : std::true_type {};

template<class Out, auto A>
constexpr bool hasTyped = HasTyped<Out, A>::value;

template<class Out, class T>
auto operator<<(Out &out, Typed<T>) -> Out & {
    return out << "<Unknown>";
}

template<class Out, auto A>
auto operator<<(Out &out, Flag<A>) -> Out & {
    return out << Typed{A};
}

template<class Out, auto... A>
auto operator<<(Out &out, Flags<A...> t) -> Out & {
    using Flags = Flags<A...>;
    if (t == Flags{}) return out << "<None>";
    t.each_set([&, first = true](auto f) mutable {
        if (!first)
            out << " | ";
        else
            first = false;
        out << f;
    });
    return out;
}

} // namespace tagvalue
