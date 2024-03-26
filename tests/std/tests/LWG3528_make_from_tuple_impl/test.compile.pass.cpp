//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//===----------------------------------------------------------------------===//

#include <array>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

struct A {
    int a;
};

struct B : public A {};

struct C {
    C(const B&) {}
};

enum class D {
    one,
    two,
};

template <class T, class Tuple>
auto can_make_from_tuple(T&&, Tuple&& t) -> decltype(std::make_from_tuple<T>(t), uint8_t()) {
    return 0;
}

template <class T, class Tuple>
uint32_t can_make_from_tuple(...) {
    return 0;
}

template <class T, class Tuple>
inline constexpr bool has_make_from_tuple_sfinae =
    std::is_same_v<decltype(can_make_from_tuple<T, Tuple>(std::declval<T>(), std::declval<Tuple>())), uint8_t>;

template <class T, class Tuple>
auto can_make_from_tuple_impl(T&&, Tuple&& t)
    -> decltype(std::_Make_from_tuple_impl<T>(std::forward<Tuple>(t),
                    std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{}),
        uint8_t()) {
    return 0;
}

template <class T, class Tuple>
uint32_t can_make_from_tuple_impl(...) {
    return 0;
}

template <class T, class Tuple>
inline constexpr bool has_make_from_tuple_impl_sfinae =
    std::is_same_v<decltype(can_make_from_tuple_impl<T, Tuple>(std::declval<T>(), std::declval<Tuple>())), uint8_t>;

template <class _Ty, class _Tuple,
    class _Seq = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<_Tuple>>>, class = void>
inline constexpr bool has_make_from_tuple = false;

template <class _Ty, class _Tuple, size_t... _Indices>
inline constexpr bool has_make_from_tuple<_Ty, _Tuple, std::index_sequence<_Indices...>,
    std::void_t<decltype(std::make_from_tuple<_Ty>(std::declval<_Tuple>()))>> = true;

template <class _Ty, class _Tuple,
    class _Seq = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<_Tuple>>>, class = void>
inline constexpr bool has_make_from_tuple_impl = false;

template <class _Ty, class _Tuple, size_t... _Indices>
inline constexpr bool has_make_from_tuple_impl<_Ty, _Tuple, std::index_sequence<_Indices...>,
    std::void_t<decltype(std::_Make_from_tuple_impl<_Ty>(std::declval<_Tuple>(),
        std::declval<std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<_Tuple>>>>()))>> = true;

// Test std::make_from_tuple.

// reinterpret_cast && std::tuple<T> && partial specialization
static_assert(!has_make_from_tuple<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple<A*, std::tuple<A*>>);

// reinterpret_cast && std::array<T, 1> && partial specialization
static_assert(!has_make_from_tuple<int*, std::array<A*, 1>>);
static_assert(has_make_from_tuple<A*, std::array<A*, 1>>);

// reinterpret_cast && std::tuple<T> && SFINAE
static_assert(!has_make_from_tuple_sfinae<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple_sfinae<A*, std::tuple<A*>>);

// reinterpret_cast && std::array<T, 1> && SFINAE
static_assert(!has_make_from_tuple_sfinae<int*, std::array<A*, 1>>);
static_assert(has_make_from_tuple_sfinae<A*, std::array<A*, 1>>);

// const_cast && std::tuple<T> && partial specialization
static_assert(!has_make_from_tuple<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple<const volatile char*, std::tuple<volatile char*>>);

// const_cast && std::array<T, 1> && partial specialization
static_assert(!has_make_from_tuple<char*, std::array<const char*, 1>>);
static_assert(!has_make_from_tuple<volatile char*, std::array<const volatile char*, 1>>);
static_assert(has_make_from_tuple<volatile char*, std::array<volatile char*, 1>>);
static_assert(has_make_from_tuple<char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple<const char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple<const volatile char*, std::array<volatile char*, 1>>);

// const_cast && std::tuple<T> && SFINAE
static_assert(!has_make_from_tuple_sfinae<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple_sfinae<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple_sfinae<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple_sfinae<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_sfinae<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_sfinae<const volatile char*, std::tuple<volatile char*>>);

// const_cast && std::array<T, 1> && SFINAE
static_assert(!has_make_from_tuple_sfinae<char*, std::array<const char*, 1>>);
static_assert(!has_make_from_tuple_sfinae<volatile char*, std::array<const volatile char*, 1>>);
static_assert(has_make_from_tuple_sfinae<volatile char*, std::array<volatile char*, 1>>);
static_assert(has_make_from_tuple_sfinae<char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple_sfinae<const char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple_sfinae<const volatile char*, std::array<volatile char*, 1>>);

// static_cast && std::tuple<T> && partial specialization
static_assert(!has_make_from_tuple<int, std::tuple<D>>);
static_assert(!has_make_from_tuple<D, std::tuple<int>>);
static_assert(has_make_from_tuple<long, std::tuple<int>>);
static_assert(has_make_from_tuple<double, std::tuple<float>>);
static_assert(has_make_from_tuple<float, std::tuple<double>>);

// static_cast && std::array<T, 1> && partial specialization
static_assert(!has_make_from_tuple<int, std::array<D, 1>>);
static_assert(!has_make_from_tuple<D, std::array<int, 1>>);
static_assert(has_make_from_tuple<long, std::array<int, 1>>);
static_assert(has_make_from_tuple<double, std::array<float, 1>>);
static_assert(has_make_from_tuple<float, std::array<double, 1>>);

// static_cast && std::tuple<T> && SFINAE
static_assert(!has_make_from_tuple_sfinae<int, std::tuple<D>>);
static_assert(!has_make_from_tuple_sfinae<D, std::tuple<int>>);
static_assert(has_make_from_tuple_sfinae<long, std::tuple<int>>);
static_assert(has_make_from_tuple_sfinae<double, std::tuple<float>>);
static_assert(has_make_from_tuple_sfinae<float, std::tuple<double>>);

// static_cast && std::array<T, 1> && SFINAE
static_assert(!has_make_from_tuple_sfinae<int, std::array<D, 1>>);
static_assert(!has_make_from_tuple_sfinae<D, std::array<int, 1>>);
static_assert(has_make_from_tuple_sfinae<long, std::array<int, 1>>);
static_assert(has_make_from_tuple_sfinae<double, std::array<float, 1>>);
static_assert(has_make_from_tuple_sfinae<float, std::array<double, 1>>);

// Test std::__Make_from_tuple_impl.

// reinterpret_cast && std::tuple<T> && partial specialization
static_assert(!has_make_from_tuple_impl<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple_impl<A*, std::tuple<A*>>);

// reinterpret_cast && std::array<T, 1> && partial specialization
static_assert(!has_make_from_tuple_impl<int*, std::array<A*, 1>>);
static_assert(has_make_from_tuple_impl<A*, std::array<A*, 1>>);

// reinterpret_cast && std::tuple<T> && SFINAE
static_assert(!has_make_from_tuple_impl_sfinae<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple_impl_sfinae<A*, std::tuple<A*>>);

// reinterpret_cast && std::array<T, 1> && SFINAE
static_assert(!has_make_from_tuple_impl_sfinae<int*, std::array<A*, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<A*, std::array<A*, 1>>);

// const_cast && std::tuple<T> && partial specialization
static_assert(!has_make_from_tuple_impl<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple_impl<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple_impl<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple_impl<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_impl<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_impl<const volatile char*, std::tuple<volatile char*>>);

// const_cast && std::array<T, 1> && partial specialization
static_assert(!has_make_from_tuple_impl<char*, std::array<const char*, 1>>);
static_assert(!has_make_from_tuple_impl<volatile char*, std::array<const volatile char*, 1>>);
static_assert(has_make_from_tuple_impl<volatile char*, std::array<volatile char*, 1>>);
static_assert(has_make_from_tuple_impl<char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple_impl<const char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple_impl<const volatile char*, std::array<volatile char*, 1>>);

// const_cast && std::tuple<T> && SFINAE
static_assert(!has_make_from_tuple_impl_sfinae<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple_impl_sfinae<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple_impl_sfinae<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple_impl_sfinae<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_impl_sfinae<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_impl_sfinae<const volatile char*, std::tuple<volatile char*>>);

// const_cast && std::array<T, 1> && SFINAE
static_assert(!has_make_from_tuple_impl_sfinae<char*, std::array<const char*, 1>>);
static_assert(!has_make_from_tuple_impl_sfinae<volatile char*, std::array<const volatile char*, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<volatile char*, std::array<volatile char*, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<const char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<const volatile char*, std::array<volatile char*, 1>>);

// static_cast && std::tuple<T> && partial specialization
static_assert(!has_make_from_tuple_impl<int, std::tuple<D>>);
static_assert(!has_make_from_tuple_impl<D, std::tuple<int>>);
static_assert(has_make_from_tuple_impl<long, std::tuple<int>>);
static_assert(has_make_from_tuple_impl<double, std::tuple<float>>);
static_assert(has_make_from_tuple_impl<float, std::tuple<double>>);

// static_cast && std::array<T, 1> && partial specialization
static_assert(!has_make_from_tuple_impl<int, std::array<D, 1>>);
static_assert(!has_make_from_tuple_impl<D, std::array<int, 1>>);
static_assert(has_make_from_tuple_impl<long, std::array<int, 1>>);
static_assert(has_make_from_tuple_impl<double, std::array<float, 1>>);
static_assert(has_make_from_tuple_impl<float, std::array<double, 1>>);

// static_cast && std::tuple<T> && SFINAE
static_assert(!has_make_from_tuple_impl_sfinae<int, std::tuple<D>>);
static_assert(!has_make_from_tuple_impl_sfinae<D, std::tuple<int>>);
static_assert(has_make_from_tuple_impl_sfinae<long, std::tuple<int>>);
static_assert(has_make_from_tuple_impl_sfinae<double, std::tuple<float>>);
static_assert(has_make_from_tuple_impl_sfinae<float, std::tuple<double>>);

// static_cast && std::array<T, 1> && SFINAE
static_assert(!has_make_from_tuple_impl_sfinae<int, std::array<D, 1>>);
static_assert(!has_make_from_tuple_impl_sfinae<D, std::array<int, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<long, std::array<int, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<double, std::array<float, 1>>);
static_assert(has_make_from_tuple_impl_sfinae<float, std::array<double, 1>>);
