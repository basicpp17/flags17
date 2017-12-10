#pragma once
#include <cstddef>
#include <limits>
#include <utility>

namespace meta::details {

template<class Tag>
struct BitStorage;

template<>
struct BitStorage<unsigned int> {
    using This = BitStorage;
    using Index = size_t;

    explicit constexpr BitStorage(Index idx) noexcept
        : v(1 << idx) {}

    template<size_t... I>
    explicit constexpr BitStorage(std::index_sequence<I...>) noexcept
        : v((0 | ... | (1 << I))) {}

    explicit constexpr BitStorage(std::initializer_list<size_t> i) noexcept
        : v([=] {
            auto s = size_t{};
            for (auto v : i) s |= (1 << v);
            return s;
        }()) {}

    constexpr BitStorage() noexcept = default;
    constexpr BitStorage(const This &) noexcept = default;
    constexpr BitStorage(This &&) noexcept = default;
    constexpr auto operator=(const This &) noexcept -> This & = default;
    constexpr auto operator=(This &&) noexcept -> This & = default;

    constexpr bool operator==(const This &o) const noexcept { return v == o.v; }
    constexpr bool operator!=(const This &o) const noexcept { return !(*this == o); }

    constexpr bool operator[](Index idx) const noexcept { return (v >> idx) & 1; }

    constexpr auto set(Index idx) const noexcept -> This { return *this | idx; }
    constexpr auto reset(Index idx) const noexcept -> This { return *this & ~BitStorage{idx}; }
    constexpr auto flip(Index idx) const noexcept -> This { return *this ^ idx; }

    constexpr static auto setAll() noexcept -> This {
        auto r = This{};
        r.v = std::numeric_limits<decltype(v)>::max();
        return r;
    }
    constexpr static auto resetAll() noexcept -> This { return This{}; }
    constexpr auto flipAll() const noexcept -> This { return ~*this; }

    constexpr auto operator~() const noexcept -> This {
        auto r = This{};
        r.v = ~v;
        return r;
    }

    constexpr auto operator|(This o) const noexcept -> This {
        auto r = This{};
        r.v = v | o.v;
        return r;
    }
    constexpr auto operator|(Index idx) const noexcept -> This {
        auto r = This{};
        r.v = v | (1 << idx);
        return r;
    }

    constexpr auto operator&(This o) const noexcept -> This {
        auto r = This{};
        r.v = v & o.v;
        return r;
    }
    constexpr auto operator&(Index idx) const noexcept -> This {
        auto r = This{};
        r.v = v & (1 << idx);
        return r;
    }

    constexpr auto operator^(This o) const noexcept -> This {
        auto r = This{};
        r.v = v ^ o.v;
        return r;
    }
    constexpr auto operator^(Index idx) const noexcept -> This {
        auto r = This{};
        r.v = v ^ (1 << idx);
        return r;
    }

private:
    unsigned int v{};
};

template<size_t bits>
constexpr auto SelectBitStorage() {
    constexpr auto bytes = (bits + 7) / 8;
    if constexpr (bytes <= sizeof(unsigned int)) {
        return BitStorage<unsigned int>{};
    }
    throw "Not enough bits in an unsigned int (TODO: write additional storage)";
}

} // namespace meta::details
