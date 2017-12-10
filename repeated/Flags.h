#pragma once
#include "meta/Value.h"
#include "meta/ValueList.h"

#include "meta/details/BitStorage.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace repeated {

#define ENABLE_REPEATED_FLAGS_OP(T)                                                                                    \
    constexpr auto operator|(T::EnumType e1, T::EnumType e2) noexcept->T { return T(e1) | e2; }

template<auto... A>
struct FlagList {};

template<auto... A>
struct Flags {
    using This = Flags;
    using Options = meta::ValueList<A...>;
    static_assert(Options::isSameType, "all Flag values shoud have same type");
    static_assert(Options::isSet, "Flag bit values have to be unique");
    using EnumType = typename Options::commonType;
    static_assert(std::is_enum_v<EnumType>, "use Flags with enum types");

    using UnderlyingType = typename Options::underlyingType;
    static constexpr auto options = Options{};
    static constexpr auto min = static_cast<UnderlyingType>(options.min);
    static constexpr auto max = static_cast<UnderlyingType>(options.max);
    static constexpr auto bitCount = 1 + max - min;

    constexpr static auto indexOf(EnumType b) noexcept -> size_t { return static_cast<UnderlyingType>(b) - min; }

    template<class... Args>
    constexpr Flags(EnumType v, Args... args) noexcept
        : storage({indexOf(v), indexOf(args)...}) {}

    template<auto... B>
    constexpr Flags(FlagList<B...> = {}) noexcept
        : storage(std::index_sequence<indexOf(B)...>{}) {}

    constexpr Flags() noexcept = default;
    ~Flags() noexcept = default;
    constexpr Flags(const This &) noexcept = default;
    constexpr Flags(This &&) noexcept = default;
    constexpr auto operator=(const This &) noexcept -> This & = default;
    constexpr auto operator=(This &&) noexcept -> This & = default;

    constexpr bool operator==(const This &o) const noexcept { return storage == o.storage; }
    constexpr bool operator!=(const This &o) const noexcept { return !(*this == o); }

    constexpr bool operator[](EnumType b) const noexcept { return storage[indexOf(b)]; }

    constexpr bool all() const noexcept { return all(setAll()); }
    constexpr bool all(This b) const noexcept { return (storage & b.storage) == b.storage; }
    template<auto B, auto... C>
    constexpr bool all(FlagList<B, C...> b = {}) const noexcept {
        static_assert(((bitCount > indexOf(B)) && ... && (bitCount > indexOf(C))));
        return all(This{b});
    }
    template<class... Args>
    constexpr bool all(EnumType b, Args... args) const noexcept {
        return all(build(b, args...));
    }

    constexpr bool any() const noexcept { return any(setAll()); }
    constexpr bool any(This b) const noexcept { return (storage & b.storage) != Storage{}; }
    template<auto B, auto... C>
    constexpr bool any(FlagList<B, C...> b = {}) const noexcept {
        static_assert(((bitCount > indexOf(B)) && ... && (bitCount > indexOf(C))));
        return any(This{b});
    }
    template<class... Args>
    constexpr bool any(EnumType b, Args... args) const noexcept {
        return any(build(b, args...));
    }

    constexpr bool none() const noexcept { return any(setAll()); }
    constexpr bool none(This b) const noexcept { return (storage & b.storage) == Storage{}; }
    template<auto B, auto... C>
    constexpr bool none(FlagList<B, C...> b = {}) const noexcept {
        static_assert(((bitCount > indexOf(B)) && ... && (bitCount > indexOf(C))));
        return none(This{b});
    }
    template<class... Args>
    constexpr bool none(EnumType b, Args... args) const noexcept {
        return none(build(b, args...));
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
        static_assert((true && ... && (bitCount > indexOf(B))));
        return set(This{b});
    }
    template<class... Args>
    constexpr auto set(EnumType b, Args... args) const noexcept -> This {
        return set(build(b, args...));
    }

    constexpr auto reset(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage & ~b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto reset(FlagList<B...> b = {}) const noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf(B))));
        return reset(This{b});
    }
    template<class... Args>
    constexpr auto reset(EnumType b, Args... args) const noexcept -> This {
        return reset(build(b, args...));
    }

    constexpr auto flip(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage ^ b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto flip(FlagList<B...> b = {}) noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf(B))));
        return flip(This{b});
    }
    template<class... Args>
    constexpr auto flip(EnumType b, Args... args) const noexcept -> This {
        return flip(build(b, args...));
    }

    constexpr auto mask(This b) const noexcept -> This {
        auto r = This{};
        r.storage = storage & b.storage;
        return r;
    }
    template<auto... B>
    constexpr auto mask(FlagList<B...> b = {}) noexcept -> This {
        static_assert((true && ... && (bitCount > indexOf(B))));
        return mask(This{b});
    }
    template<class... Args>
    constexpr auto mask(EnumType b, Args... args) const noexcept -> This {
        return mask(build(b, args...));
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
    constexpr auto operator|=(B b) const noexcept -> This & {
        return *this = set(b);
    }
    template<class B>
    constexpr auto operator&=(B b) const noexcept -> This & {
        return *this = mask(b);
    }
    template<class B>
    constexpr auto operator^=(B b) const noexcept -> This & {
        return *this = flip(b);
    }

    template<class F>
    constexpr void each(F &&f) const noexcept {
        options.each([&, ff = std::forward<F>(f) ](auto value) mutable {
            auto flag = value.value();
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
    template<class... Args>
    static constexpr auto build(Args... args) noexcept -> Flags {
        return Flags{args...};
    }

private:
    using Storage = decltype(meta::details::SelectBitStorage<bitCount>());
    Storage storage{};
};

namespace test {

enum class TE { n1, n2, n3 = 4 };
static_assert(Flags<TE::n1, TE::n2, TE::n3>{TE::n1, TE::n2} == FlagList<TE::n1, TE::n2>{}, "");
static_assert((Flags<TE::n1, TE::n2>{} | TE::n1) == FlagList<TE::n1>{}, "");
static_assert((Flags<TE::n1, TE::n2, TE::n3>{}.set(TE::n1, TE::n3) & FlagList<TE::n1, TE::n2>{}) == FlagList<TE::n1>{},
              "");
static_assert(Flags<TE::n1, TE::n2, TE::n3>{}.flip(TE::n2, TE::n3).any(TE::n1, TE::n2), "");
static_assert((Flags<TE::n1, TE::n2, TE::n3>::setAll() & TE::n2) == FlagList<TE::n2>{}, "not all set");

} // namespace test

template<class Out, auto... A>
auto operator<<(Out &out, Flags<A...> t) -> Out & {
    using Flags = Flags<A...>;
    if (t == Flags{}) return out << "<None>";
    t.each_set([&, first = true ](auto f) mutable {
        if (!first)
            out << " | ";
        else
            first = false;
        out << f;
    });
    return out;
}

} // namespace repeated
