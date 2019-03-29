
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <numeric>
#include <random>
#include <type_traits>

#include <sax/splitmix.hpp> // https://github.com/degski/Sax/blob/master/splitmix.hpp
#include "uniform_int_distribution_fast.hpp"
#include "singleton.hpp"
#include "moves.hpp"


extern Singleton<sax::splitmix64> rng;


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

using MovesType = Moves<MoveType, 64>;


[[ nodiscard ]] MovesType getMoves ( ) noexcept {
    MovesType moves;
    moves.size ( ) = moves.capacity ( );
    std::iota<MoveType*, std::uint8_t> ( std::begin ( moves ), std::end ( moves ), 0u );
    std::shuffle ( std::begin ( moves ), std::end ( moves ), rng.instance ( ) );
    return moves;
}

template<typename Tree, typename N>
[[ maybe_unused ]] N addChild ( Tree & tree_, const N source_ ) noexcept {
    const N target = tree_.addNode ( getMoves ( ) );
    tree_.addArc ( source_, target, tree_ [ source_ ].take ( ) );
    return target;
}

template<typename Tree, typename N>
void addLink ( Tree & tree_, const N source_, const N target_ ) noexcept {
    tree_.addArc ( source_, target_, tree_.data ( source_ ).take ( ) );
}

template<typename Tree, typename N>
[[ nodiscard ]] bool hasMoves ( const Tree & tree_, const N source_ ) noexcept {
    return tree_ [ source_ ].size ( );
}

template<typename Tree, typename N>
[[ nodiscard ]] N selectChild ( const Tree & tree_, const N source_ ) noexcept {
    const std::uint32_t n = ext::uniform_int_distribution_fast<std::uint32_t> ( 0, tree_.outArcNum ( source_ ) - 1 ) ( rng.instance ( ) );
    if constexpr ( std::is_pointer<typename Tree::NodeID>::value ) { // ast.
        return tree_.outArcs ( source_ ) [ n ]->target;
    }
    else { // fst.
        typename Tree::const_out_iterator it = tree_.cbeginOut ( source_ );
        std::advance ( it, n );
        return it->target;
    }
}

template<typename Tree, typename N>
[[ nodiscard ]] bool hasChild ( const Tree & tree_, const N source_ ) noexcept {
    return tree_.hasOutArc ( source_ );
}
