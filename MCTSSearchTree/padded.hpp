
// MIT License
//
// Copyright (c) 2018, 2019 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <cstdlib>

#include <type_traits>

namespace detail {
// Integer LogN.
template<int Base, typename T, typename sfinae = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr T iLog ( const T n_, const T p_ = T ( 0 ) ) noexcept {
    return n_ < Base ? p_ : iLog<Base, T, sfinae> ( n_ / Base, p_ + 1 );
}
// Integer Log2.
template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr T ilog2 ( const T n_ ) noexcept {
    return iLog<2, T> ( n_ );
}
template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr T next_power_2 ( const T n_ ) noexcept {
    return n_ > 2 ? T ( 1 ) << ( ilog2<T> ( n_ - 1 ) + 1 ) : n_;
}
}

// Padds T to a size of a power of 2.
template<typename T, std::size_t Size = detail::next_power_2 ( sizeof ( T ) ) - sizeof ( T )>
struct Padded : public T { // Padded.
    template<typename ... Args>
    Padded ( Args && ... args_ ) : T ( std::forward<Args> ( args_ ) ... ) {
    }
    private:
    char _ [ Size ];
};
template<typename T>
struct Padded<T, 0> : public T { // No padding.
    template<typename ... Args>
    Padded ( Args && ... args_ ) : T ( std::forward<Args> ( args_ ) ... ) {
    }
};
