
// MIT License
//
// Copyright (c) 2018 degski
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <numeric>
#include <random>

#include "splitmix.hpp"
#include "uniform_int_distribution_fast.hpp"
#include "moves.hpp"
#include "singleton.hpp"


extern Singleton<splitmix64> rng;


struct MoveType {
    std::uint8_t value;

    MoveType ( ) noexcept {
    }
    MoveType ( const std::uint8_t m_ ) noexcept :
        value { m_ } {
    }
    MoveType ( std::uint8_t && m_ ) noexcept :
        value { std::move ( m_ ) } {
    }
};

using MovesType = Moves<MoveType, 256>;


[[ nodiscard ]] MovesType getMoves ( ) noexcept {
    MovesType moves;
    moves.size ( ) = moves.capacity ( );
    std::iota<MoveType*, std::uint8_t> ( std::begin ( moves ), std::end ( moves ), 0u );
    std::shuffle ( std::begin ( moves ), std::end ( moves ), rng.instance ( ) );
    return moves;
}

template<typename T, typename N>
[[ nodiscard ]] N addNode ( T & t_, N source_ ) noexcept {
    const N target = t_.addNode ( getMoves ( ) );
    t_.addArc ( source_, target, t_.data ( source_ ).take ( ) );
    return target;
}

template<typename T, typename N>
void addArc ( T & t_, N source_, N target_ ) noexcept {
    t_.addArc ( source_, target_, t_.data ( source_ ).take ( ) );
}

template<typename T, typename N>
[[ nodiscard ]] bool hasMoves ( T & t_, N source_ ) noexcept {
    return t_.data ( source_ ).size ( );
}

template<typename T, typename N>
[[ nodiscard ]] N selectChild ( T & t_, N source_ ) noexcept {
    auto it = t_.cbeginOut ( source_ );
    std::advance ( it, ext::uniform_int_distribution_fast<std::uint32_t> ( 0, t_.outArcNum ( source_ ) - 1 ) ( rng.instance ( ) ) );
    return it->target;
}

template<typename T, typename N>
[[ nodiscard ]] N selectChildVector ( T & t_, N source_ ) noexcept {
    return t_.outArcs ( source_ ) [ ext::uniform_int_distribution_fast<std::uint32_t> ( 0, t_.outArcNum ( source_ ) - 1 ) ( rng.instance ( ) ) ]->target;
}

template<typename T, typename N>
[[ nodiscard ]] bool hasChild ( T & t_, N source_ ) noexcept {
    return t_.hasOutArc ( source_ );
}
