#include <cinttypes>
#include <iostream>

namespace legacy_enum_approach {
enum Animals {
    ANIMALS_CAT = 1 << 0,
    ANIMALS_DOG = 1 << 1,
    ANIMALS_WOLF = 1 << 3,
};
static_assert(std::is_same_v<decltype(std::declval<Animals>() | std::declval<Animals>()), int>, //
              "default fallback to int");

auto operator|(Animals a, Animals b) -> Animals { return Animals(int(a) | b); }
auto operator&(Animals a, Animals b) -> Animals { return Animals(int(a) & b); }

static_assert(std::is_same_v<decltype(std::declval<Animals>() | std::declval<Animals>()), Animals>,
              "operator keeps types");

auto operator<<(std::ostream &out, Animals a) -> std::ostream & {
    auto p = [&, first = true ](const char *t) mutable {
        if (first)
            first = false;
        else
            out << " | ";
        out << t;
    };
    if (a & ANIMALS_CAT) p("Cat");
    if (a & ANIMALS_DOG) p("Dog");
    if (a & ANIMALS_WOLF) p("Wolf");
    return out;
}

void usage() {
    constexpr auto Cat = ANIMALS_CAT;
    auto a = Cat | ANIMALS_DOG;
    std::cout << Cat << '\n';
    std::cout << a << '\n';
    std::cout << ((a & Cat) | ANIMALS_WOLF) << '\n';
}
} // namespace legacy_enum_approach

#include "classic/Flags.h"
namespace classic_enum_class_approach {
enum class Animal {
    Cat = 1 << 0,
    Dog = 1 << 1,
    Wolf = 1 << 3,
};
using Animals = classic::Flags<Animal>;

// hint: use this outside of namespace or it wont be usable there!
ENABLE_CLASSIC_FLAGS_OP(Animals)

auto operator<<(std::ostream &out, Animal a) -> std::ostream & {
    switch (a) {
    case Animal::Cat: return out << "Cat";
    case Animal::Dog: return out << "Dog";
    case Animal::Wolf: return out << "Wolf";
    }
}

void usage() {
    constexpr auto Cat = Animal::Cat;
    auto a = Cat | Animal::Dog;
    std::cout << Cat << '\n';
    std::cout << a << '\n';
    std::cout << ((a & Cat) | Animal::Wolf) << '\n';
}

} // namespace classic_enum_class_approach

#include "bitnumber/Flags.h"
namespace bit_number_approach {
enum class Animal { Cat, Dog, Wolf = 3 };
using Animals = bitnumber::Flags<Animal>;

// hint: use this outside of namespace or it wont be usable there!
ENABLE_BITNUMBER_FLAGS_OP(Animals)

auto operator<<(std::ostream &out, Animal a) -> std::ostream & {
    switch (a) {
    case Animal::Cat: return out << "Cat";
    case Animal::Dog: return out << "Dog";
    case Animal::Wolf: return out << "Wolf";
    }
}

void usage() {
    constexpr auto Cat = Animal::Cat;
    auto a = Cat | Animal::Dog;
    std::cout << Cat << '\n';
    std::cout << a << '\n';
    std::cout << ((a & Cat) | Animal::Wolf) << '\n';
}
} // namespace bit_number_approach

#include "tagtype/Flags.h"
namespace tag_type_approach {

struct Animal {
    struct Cat;
    struct Dog;
    struct Wolf;
};
using Animals = tagtype::Flags<Animal::Cat, Animal::Dog, void, Animal::Wolf>;

using AnimalCat = Animal::Cat;
auto operator<<(std::ostream &out, tagtype::Flag<AnimalCat>) -> std::ostream & { return out << "Cat"; }
auto operator<<(std::ostream &out, tagtype::Flag<Animal::Dog>) -> std::ostream & { return out << "Dog"; }
auto operator<<(std::ostream &out, tagtype::Flag<Animal::Wolf>) -> std::ostream & { return out << "Wolf"; }

template<class T>
constexpr auto Flag = tagtype::Flag<T>{};

void usage() {
    using Cat = Animal::Cat;                 // use type aliases for shorter names
    constexpr auto DogF = Flag<Animal::Dog>; // flag types works only if you use them one by one
    auto a = Animals(Flag<Cat>, DogF);
    std::cout << Flag<Cat> << '\n';
    std::cout << a << '\n';
    std::cout << (a & Flag<Cat> | Flag<Animal::Wolf>) << '\n';
}
} // namespace tag_type_approach

#include "tagvalue/Flags.h"
namespace tag_value_approach {

enum class Animal { Cat, Dog };
enum class AnimalWolf {};
constexpr auto AnimalCat = Animal::Cat;

using Animals = tagvalue::Flags<AnimalCat, Animal::Dog, nullptr, AnimalWolf{}>;

#if true // one stream operator per type
using tagvalue::operator<<;
auto operator<<(std::ostream &out, tagvalue::Typed<Animal> a) -> std::ostream & {
    switch (a.value) {
    case AnimalCat: return out << "Cat";
    case Animal::Dog: return out << "Dog";
    }
}
auto operator<<(std::ostream &out, tagvalue::Typed<AnimalWolf>) -> std::ostream & { return out << "Wolf"; }
#else // individual stream operators
} // namespace tag_value_approach
namespace meta {
auto operator<<(std::ostream &out, tagvalue::Flag<tag_value_approach::AnimalCat>) -> std::ostream & {
    return out << "Cat";
}
auto operator<<(std::ostream &out, tagvalue::Flag<tag_value_approach::Animal::Dog>) -> std::ostream & {
    return out << "Dog";
}
auto operator<<(std::ostream &out, tagvalue::Flag<tag_value_approach::AnimalWolf{}>) -> std::ostream & {
    return out << "Wolf";
}
} // namespace meta
namespace tag_value_approach {
#endif

template<auto V>
constexpr auto Flag = tagvalue::Flag<V>{};

void usage() {
    constexpr auto Cat = Animal::Cat;
    constexpr auto DogF = Flag<Animal::Dog>;
    auto a = Animals(Flag<Cat>, DogF);
    std::cout << Flag<Cat> << '\n';
    std::cout << a << '\n';
    std::cout << (a & Flag<Cat> | Flag<AnimalWolf{}>) << '\n';
}
} // namespace tag_value_approach

#include "repeated/Flags.h"
namespace repeated_bit_approach {

enum class Animal { Cat, Dog, Wolf = 3 };
constexpr auto AnimalCat = Animal::Cat;

using Animals = repeated::Flags<AnimalCat, Animal::Dog, Animal::Wolf>;
ENABLE_REPEATED_FLAGS_OP(Animals)

auto operator<<(std::ostream &out, Animal a) -> std::ostream & {
    switch (a) {
    case AnimalCat: return out << "Cat";
    case Animal::Dog: return out << "Dog";
    case Animal::Wolf: return out << "Wolf";
    }
}

void usage() {
    constexpr auto Cat = Animal::Cat;
    auto a = Cat | Animal::Dog;
    std::cout << Cat << '\n';
    std::cout << a << '\n';
    std::cout << ((a & Cat) | Animal::Wolf) << '\n';
}
} // namespace repeated_bit_approach

int main() {
    std::cout << "\n-- legacy enum approach --\n";
    legacy_enum_approach::usage();

    std::cout << "\n-- classic enum class approach --\n";
    classic_enum_class_approach::usage();

    std::cout << "\n-- bit number approach --\n";
    bit_number_approach::usage();

    std::cout << "\n-- tag type approach --\n";
    tag_type_approach::usage();

    std::cout << "\n-- tag value approach --\n";
    tag_value_approach::usage();

    std::cout << "\n-- repeated bit approach --\n";
    repeated_bit_approach::usage();
}
