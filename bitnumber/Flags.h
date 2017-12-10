#pragma once
#include <cinttypes>
#include <initializer_list>
#include <type_traits>

namespace bitnumber {

#define ENABLE_BITNUMBER_FLAGS_OP(T)                                                                                   \
    constexpr auto operator|(T::EnumType e1, T::EnumType e2) noexcept->T { return T(e1) | e2; }

template<class T, class V = uint64_t>
struct Flags {
    using This = Flags;
    static_assert(std::is_enum_v<T>, "flags only works for enums!");

    using EnumType = T;
    using ValueType = std::underlying_type_t<T>;
    using BitType = V;

    constexpr Flags(T v) noexcept
        : Flags(BitType{1} << static_cast<ValueType>(v)) {}

    template<class... Args>
    constexpr Flags(T v, Args... args) noexcept
        : Flags(build(v, args...)) {}

    constexpr Flags() noexcept = default;
    ~Flags() noexcept = default;
    constexpr Flags(const This &) noexcept = default;
    constexpr Flags(This &&) noexcept = default;
    constexpr auto operator=(const This &) noexcept -> This & = default;
    constexpr auto operator=(This &&) noexcept -> This & = default;

    constexpr void swap(This &fl) noexcept { std::swap(v, fl.v); }

    constexpr bool operator==(const This &f) const noexcept { return v == f.v; }
    constexpr bool operator!=(const This &f) const noexcept { return v != f.v; }

    template<class B>
    constexpr bool operator[](T t) const noexcept {
        return (v & This{t}.v) != BitType{};
    }

    constexpr bool all(This b) const noexcept { return (v & b.v) == b.v; }
    template<class... Args>
    constexpr bool all(T b, Args... args) const noexcept {
        return all(build(b, args...));
    }

    constexpr bool any(This f) const noexcept { return (v & f.v) != BitType{}; }
    template<class... Args>
    constexpr bool any(T b, Args... args) const noexcept {
        return any(build(b, args...));
    }

    constexpr bool none() const noexcept { return v == 0; }
    constexpr bool none(This f) const noexcept { return (v & f.v) == BitType{}; }
    template<class... Args>
    constexpr bool none(T b, Args... args) const noexcept {
        return none(build(b, args...));
    }

    constexpr static auto resetAll() noexcept -> This { return Flags{}; }

    constexpr auto set(This b) const noexcept -> This { return This{v | b.v}; }
    template<class... Args>
    constexpr auto set(T b, Args... args) const noexcept -> This {
        return set(build(b, args...));
    }

    constexpr auto reset(This b) const noexcept -> This { return This{v & ~b.v}; }
    template<class... Args>
    constexpr auto reset(T b, Args... args) const noexcept -> This {
        return reset(build(b, args...));
    }

    constexpr auto flip(This b) const noexcept -> This { return This{v ^ b.v}; }
    template<class... Args>
    constexpr auto flip(T b, Args... args) const noexcept -> This {
        return flip(build(b, args...));
    }

    constexpr auto mask(This b) const noexcept -> This { return This{v & b.v}; }
    template<class... Args>
    constexpr auto mask(T b, Args... args) const noexcept -> This {
        return mask(build(b, args...));
    }

    constexpr This operator|(const This &f) const noexcept { return {v | f.v}; }

    constexpr Flags clear(const Flags &f) const noexcept { return {v & ~f.v}; }

    template<class... Args>
    constexpr bool clear(const T &v, Args &&... args) const noexcept {
        return clear(build(v, args...));
    }

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
    constexpr void each_set(F &&f) const noexcept {
        auto vt = ValueType{};
        while (true) {
            const auto t = static_cast<T>(vt);
            const auto ft = Flags{t};
            if (ft.v > v) break;
            if (any(t)) f(t);
            vt++;
        }
    }

private:
    constexpr Flags(BitType v) noexcept
        : v(v) {}

    template<class... Args>
    static constexpr auto build(Args... args) noexcept -> Flags {
        return Flags{(... | Flags{args})};
    }

private:
    BitType v{};
};

template<class Out, class T>
auto operator<<(Out &out, Flags<T> f)
    -> std::enable_if_t<std::is_same_v<decltype(out << std::declval<T>()), decltype(out)>, decltype(out)> //
{
    using flags_type = Flags<T>;
    if (f == flags_type{}) return out << "<None>";
    f.each_set([&, first = true ](T t) mutable {
        if (!first)
            out << " | ";
        else
            first = false;
        out << t;
    });
    return out;
}

} // namespace bitnumber
