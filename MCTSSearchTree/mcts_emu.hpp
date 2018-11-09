
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
#include "moves.hpp"


extern splitmix64 rng;


struct MoveType {
    std::uint8_t move;

    explicit MoveType ( ) noexcept {
    }
    explicit MoveType ( const std::uint8_t m_ ) noexcept :
        move { m_ } {
    }
    explicit MoveType ( std::uint8_t && m_ ) noexcept :
        move { std::move ( m_ ) } {
    }
};

using MovesType = Moves<MoveType, 256>;


MovesType getMoves ( ) {
    MovesType moves;
    moves.size ( ) = moves.capacity ( );
    std::iota<MoveType*, std::uint8_t> ( std::begin ( moves ), std::end ( moves ), 0 );
    std::shuffle ( std::begin ( moves ), std::end ( moves ), rng );
    return moves;
}

template<typename G, typename N>
N addNode ( G & g_, N source_, const bool print = false ) {
    N target = g_.addNode ( getMoves ( ) );
    g_.addArc ( source_, target, g_.data ( source_ ).draw ( ) );
    return target;
}

template<typename G, typename N>
void addArc ( G & g_, N source_, N target_, const bool print = false ) {
    g_.addArc ( source_, target_, g_.data ( source_ ).draw ( ) );
}

template<typename G, typename N>
bool hasMoves ( G & g_, N source_ ) noexcept {
    return g_.data ( source_ ).size ( );
}

template<typename G, typename N>
N selectChild ( G & g_, N source_ ) {
    auto it = g_.cbeginOut ( source_ );
    std::advance ( it, std::uniform_int_distribution<std::uint32_t> ( 0, g_.outArcNum ( source_ ) - 1 ) ( rng ) );
    return it->target;
}

template<typename G, typename N>
N selectChildVector ( G & g_, N source_ ) {
    return g_.outArcs ( source_ ) [ std::uniform_int_distribution<std::uint32_t> ( 0, g_.outArcNum ( source_ ) - 1 ) ( rng ) ]->target;
}

template<typename G, typename N>
bool hasChild ( G& g_, N source_ ) {
    return g_.hasOutArc ( source_ );
}
