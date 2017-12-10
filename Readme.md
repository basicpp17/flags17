# C++17 Flags

### Summary of flags with C++17.

For the new experimental compiler of Rebuild language project I needed all kinds of flags.
This ended up being a rabbit hole. Here is what I digged out of that.

## Legacy enum approach

Basically taken from C to C++.

```cpp
enum Animals {
    ANIMALS_CAT = 1 << 0,
    ANIMALS_DOG = 1 << 1,
    ANIMALS_WOLF = 1 << 3,
};
// define operator to prevent fallback to int
auto operator|(Animals a, Animals b) -> Animals { return Animals(int(a) | b); }
auto operator&(Animals a, Animals b) -> Animals { return Animals(int(a) & b); }

// define stream operator for bitmask
auto operator<<(std::ostream &out, Animals a) -> std::ostream & {
    // …see main.cpp for details
}

void usage() {
    auto a = ANIMALS_CAT | ANIMALS_DOG;
    std::cout << a << '\n';
    std::cout << ((a & ANIMALS_CAT) | ANIMALS_WOLF) << '\n';
}
```

References:
* C-APIs: Posix, ffmpeg, win32 …

Advantages:
* very well known pattern
* syntax compatible with C

Disadvantages:
* enum value names leak to namespace
* programmer has to ensure that all bits are unique
* hard to spot skipped bits
* no iteration on bits possible
* no type safety (easy to mix different masks)


## Classical `enum class` approach

The entries inside the `enum class` are the bit mask for each flag.

```cpp
enum class Animal {
    Cat = 1 << 0,
    Dog = 1 << 1,
    Wolf = 1 << 3,
};
using Animals = classic::Flags<Animal>;

ENABLE_CLASSIC_FLAGS_OP(Animals)

// stream operator for a single value
auto operator<<(std::ostream &out, Animal a) -> std::ostream & {
    // …see main.cpp for details
}

void usage() {
    auto a = Animal::Cat | Animal::Dog;
    std::cout << a << '\n';
    std::cout << ((a & Animal::Cat) | Animal::Wolf) << '\n';
}
```

References:
* Qt framework: `QFlags<T>`
* most of the flags helpers out there

Advantages:
* many C and C++ programmers are familiar with this approach.
* programmer sees everything
* programmer controls underlying data type
* allows skipped bits

Disadvantages:
* programmer has to ensure that all bits are unique
* hard to spot skipped bits
* no iteration over unset bits possible

Restrictions:
* can be implemented in any C++ version
* benefits from C++11 `enum class` but requires macro or type trait to allow `operator |` for enum values


## Bit number based approach

**New** modification of the classical `enum class` approach.\
The entries contain the bit number of the flag.

```cpp
enum class Animal { Cat, Dog, Wolf = 3 };
using Animals = bitnumber::Flags<Animal>;

ENABLE_BITNUMBER_FLAGS_OP(Animals)

// stream operator for a single value
auto operator<<(std::ostream &out, Animal a) -> std::ostream & {
    // …see main.cpp for details
}

void usage() {
    auto a = Animal::Cat | Animal::Dog;
    std::cout << a << '\n';
    std::cout << ((a & Animal::Cat) | Animal::Wolf) << '\n';
}
```

References:
* **new** *(I could not find an existing implementation)*

Advantages:
* automatic enum values work
* programmer still has control
* allows skipped bits

Disadvantages:
* programmer has to ensure all bits are unique
* no iteration over unset bits possible
* flag storage data type is disconnected
* no way to ensure all flags fit

Restrictions:
* same as classical approach


## Tag type based approach

All flags are represented by tag types.

_Hint_: A tag type is a type we only use to get a unique name.

```cpp
struct Animal {
    struct Cat;
    struct Dog;
    struct Wolf;
};
using Animals = tagtype::Flags<Animal::Cat, Animal::Dog, void, Animal::Wolf>;

auto operator<<(std::ostream &out, tagtype::Flag<Animal::Cat>) -> std::ostream & {
    return out << "Cat";
} // … repeated for other types

void usage() {
    auto a = Animals(Flag<Animal::Cat>, Flag<Animal::Dog>);
    std::cout << a << '\n';
    std::cout << (a & Flag<Animal::Cat> | Flag<Animal::Wolf>) << '\n';
}
```

References:
* Compmaniak's `typed_flags<…>` see https://github.com/compmaniak/typed_flags

Advantages:
* full encapsulation
* flags manages bit layout
* flag type knows all possible flags
* can iterate over all possible flags
* types are quite flexible

Disadvantages:
* tags have to be repeated
* less control over bit layout
* hard to deprecate flags or skip bits
* tag types are no values

Restrictions:
* requires C++11 `template<class...>` parameter packs
* benefits from C++17 fold expressions - http://en.cppreference.com/w/cpp/language/fold


## Tag value based approach

Each flag is represented by a tag value.

```cpp
enum class Animal { Cat, Dog };
enum class AnimalWolf {}; // any type allowed

using Animals = tagvalue::Flags<Animal::Cat, Animal::Dog, nullptr, AnimalWolf{}>;

using tagvalue::operator<<;
auto operator<<(std::ostream &out, tagvalue::Typed<Animal> a) -> std::ostream & {
    // … see main.cpp
}
auto operator<<(std::ostream &out, tagvalue::Typed<AnimalWolf>) -> std::ostream & {
    return out << "Wolf";
}

void usage() {
    auto a = Animals(Flag<Animal::Cat>, Flag<Animal::Dog>);
    std::cout << a << '\n';
    std::cout << (a & Flag<Animal::Cat> | Flag<AnimalWolf{}>) << '\n';
}
```

References:
* *new* (I could not find any usages of this approach)

Advantages:
* full encapsulation
* no restrictions to the values (they are just tags)
* can iterate over all possible flags

Disadvantages:
* tags have to be repeated
* less control over bit layout

Restrictions:
* requires the C++17 `template<auto V>` feature


## Repeated bit value approach

The underlying values of the tags are used as bit number.

```cpp
enum class Animal { Cat, Dog, Wolf = 3 };
using Animals = repeated::Flags<Animal::Cat, Animal::Dog, Animal::Wolf>
ENABLE_REPEATED_FLAGS_OP(Animals)

auto operator<<(std::ostream &out, Animal a) -> std::ostream & {
    // … see main.cpp
}

void usage() {
    auto a = Animal::Cat | Animal::Dog;
    std::cout << a << '\n';
    std::cout << ((a & Animal::Cat) | Animal::Wolf) << '\n';
}
```

References:
* *new* (I could not find any usages of this approach)

Advantages:
* full encapsulation
* underlying type will always fit
* duplicate bit values lead to compile error
* can iterate over all possible flags

Disadvantages:
* enum values have to be repeated

Restrictions:
* requires the C++17 `template<auto V>` feature
  * C++14 requires to repeat the enum type as well. `repeated<Animal, Animal::Cat, …>`


## Summary

There is no perfect solution in C++.
C++17 makes some new variants more viable.

## Contribute

Feel free to add PR for improvements or new flag implementations.

## License

Apache License

see LICENSE file for details.