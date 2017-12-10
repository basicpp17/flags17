#include <cinttypes>

namespace meta::details {

struct BitIntrinsics {
    // Returns number of zero bits preceding least significant 1 bit.
    // Undefined for zero value.
    static inline int countOfTrailingZeros(uint32_t);

    // Returns number of zero bits following most significant 1 bit.
    // Undefined for zero value.
    static inline int countLeadingZeros(uint32_t);

    // Returns the number of bits set.
    static inline int countSetBits(uint32_t);
};

#ifdef __GNUC__
int BitIntrinsics::countOfTrailingZeros(uint32_t value) { return __builtin_ctz(value); }

int BitIntrinsics::countLeadingZeros(uint32_t value) { return __builtin_clz(value); }

int BitIntrinsics::countSetBits(uint32_t value) { return __builtin_popcount(value); }

#elif defined(_MSC_VER)

#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)

int BitIntrinsics::countOfTrailingZeros(uint32_t value) {
    unsigned long result;            // NOLINT
    _BitScanForward(&result, value); // NOLINT
    return static_cast<int>(result);
}

int BitIntrinsics::countLeadingZeros(uint32_t value) {
    unsigned long result;            // NOLINT
    _BitScanReverse(&result, value); // NOLINT
    return 31 - static_cast<int>(result);
}

int BitIntrinsics::countSetBits(uint32_t value) {
    // Manually count set bits.
    value = ((value >> 1) & 0x55555555) + (value & 0x55555555);
    value = ((value >> 2) & 0x33333333) + (value & 0x33333333);
    value = ((value >> 4) & 0x0f0f0f0f) + (value & 0x0f0f0f0f);
    value = ((value >> 8) & 0x00ff00ff) + (value & 0x00ff00ff);
    value = ((value >> 16) & 0x0000ffff) + (value & 0x0000ffff);
    return value;
}

#else
#error Unsupported compiler
#endif
/*
template<uint32_t n>
constexpr auto count_zeros_impl(uint32_t v) -> uint32_t {
    if constexpr (n == 0) return 0;
    if constexpr (n == 1) return !(v & 1);
    constexpr auto h = n / 2;
    constexpr auto hm = (1<<h)-1;
    auto lower = v & hm;
    if (lower) return count_zeros_impl<h>(lower);
    else return h + count_zeros_impl<n - h>(v >> h);
}

static_assert(count_zeros_impl<5>(15) == 0);
static_assert(count_zeros_impl<5>(8) == 3);
static_assert(count_zeros_impl<5>(4) == 2);
static_assert(count_zeros_impl<5>(2) == 1);
static_assert(count_zeros_impl<5>(1) == 0);
static_assert(count_zeros_impl<32>(1<<24) == 24);
*/
auto count_zeros(uint32_t v) -> int {
    // return count_zeros_impl<4>(v);
    if (v)
        return BitIntrinsics::countOfTrailingZeros(v);
    else
        return 0;
}

} // namespace meta::details
