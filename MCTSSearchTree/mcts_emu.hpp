
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


extern splitmix64 rng;


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
    std::shuffle ( std::begin ( moves ), std::end ( moves ), rng );
    return moves;
}

template<typename G, typename N>
[[ nodiscard ]] N addNode ( G & g_, N source_, const bool print = false ) noexcept {
    N target = g_.addNode ( getMoves ( ) );
    const auto _ { g_.addArc ( source_, target, g_.data ( source_ ).take ( ) ) };
    return target;
}

template<typename G, typename N>
void addArc ( G & g_, N source_, N target_, const bool print = false ) noexcept {
    const auto _ { g_.addArc ( source_, target_, g_.data ( source_ ).take ( ) ) };
}

template<typename G, typename N>
[[ nodiscard ]] bool hasMoves ( G & g_, N source_ ) noexcept {
    return g_.data ( source_ ).size ( );
}

template<typename G, typename N>
[[ nodiscard ]] N selectChild ( G & g_, N source_ ) noexcept {
    auto it = g_.cbeginOut ( source_ );
    std::advance ( it, ext::uniform_int_distribution_fast<std::uint32_t> ( 0, g_.outArcNum ( source_ ) - 1 ) ( rng ) );
    return it->target;
}

template<typename G, typename N>
[[ nodiscard ]] N selectChildVector ( G & g_, N source_ ) noexcept {
    return g_.outArcs ( source_ ) [ ext::uniform_int_distribution_fast<std::uint32_t> ( 0, g_.outArcNum ( source_ ) - 1 ) ( rng ) ]->target;
}

template<typename G, typename N>
[[ nodiscard ]] bool hasChild ( G& g_, N source_ ) noexcept {
    return g_.hasOutArc ( source_ );
}
