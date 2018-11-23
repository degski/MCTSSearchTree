
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
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <optional>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/pector.hpp>

#include "splitmix.hpp"


template<typename Tree>
struct Transistion {

    using ArcID = typename Tree::ArcID;
    using NodeID = typename Tree::NodeID;

    ArcID arc;
    NodeID target;

    Transistion ( ) noexcept {
    }
    Transistion ( Transistion && l_ ) noexcept :
        arc { std::move ( l_.arc ) },
        target { std::move ( l_.target ) } {
    }
    Transistion ( const Transistion & l_ ) noexcept :
        arc { l_.arc },
        target { l_.target } {
    }
    Transistion ( const ArcID a_, const NodeID t_ ) noexcept :
        arc { a_ },
        target { t_ } {
    }
    Transistion ( const ArcID a_ ) noexcept :
        arc { a_ } {
    }
    Transistion ( const NodeID t_ ) noexcept :
        target { t_ } {
    }

    [[ nodiscard ]] bool operator == ( const Transistion & rhs_ ) const noexcept {
        return arc == rhs_.arc and target == rhs_.target;
    }
    [[ nodiscard ]] bool operator != ( const Transistion & rhs_ ) const noexcept {
        return arc != rhs_.arc or target != rhs_.target;
    }

    [[ maybe_unused ]] Transistion & operator = ( const Transistion & l_ ) noexcept {
        arc = l_.arc;
        target = l_.target;
        return * this;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Transistion & l_ ) noexcept {
        out_ << L'<' << l_.arc << L' ' << l_.target << L'>';
        return out_;
    }

    private:

    friend class cereal::access;

    template < class Archive >
    void serialize ( Archive & ar_ ) {
        ar_ ( arc, target );
    }
};

template<typename Tree>
using OptionalLink = std::optional<Transistion<Tree>>;
