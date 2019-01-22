
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
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <random>
#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "splitmix.hpp"
#include "uniform_int_distribution_fast.hpp"
#include "types.hpp"
#include "singleton.hpp"


extern Singleton<splitmix64> rng;


template<typename T, std::size_t S>
class Moves {

    typedef T Array [ S ]; // Type-deffing a C-array.

    Int m_size = 0;
    Array m_moves;

    public:

    Moves ( ) noexcept {
    }

    using value_type = T;

    void clear ( ) noexcept {
        m_size = 0;
    }

    // Assignment is allowed! Yes, it's weird.
    [[ nodiscard ]] Int & size ( ) noexcept {
        return m_size;
    }

    [[ nodiscard ]] Int size ( ) const noexcept {
        return m_size;
    }

    [[ nodiscard ]] Int capacity ( ) const noexcept {
        return S;
    }

    [[ nodiscard ]] bool empty ( ) const noexcept {
        return not ( m_size );
    }

    [[ nodiscard ]] value_type at ( const Int i_ ) const noexcept {
        assert ( i_ >= 0 );
        assert ( i_ < size ( ) );
        return m_moves [ i_ ];
    }

    [[ nodiscard ]] value_type front ( ) const noexcept {
        return m_moves [ 0 ];
    }

    void push_back ( const value_type m_ ) noexcept {
        m_moves [ m_size++ ] = m_;
    }

    void emplace_back ( value_type && m_ ) noexcept {
        m_moves [ m_size++ ] = std::move ( m_ );
    }

    [[ nodiscard ]] value_type random ( ) const noexcept {
        return m_moves [ ext::uniform_int_distribution_fast<std::ptrdiff_t> ( 0, m_size - 1 ) ( rng.instance ( ) ) ];
    }

    [[ nodiscard ]] bool find ( const value_type m_ ) const noexcept {
        for ( Int i = 0; i < m_size; ++i ) {
            if ( m_moves [ i ] == m_ ) {
                return true;
            }
        }
        return false;
    }

    // Select a move, remove and return it.
    [[ nodiscard ]] value_type take ( ) noexcept {
        const Int i { ext::uniform_int_distribution_fast<Int> { 0, --m_size } ( rng.instance ( ) ) };
        const value_type v { m_moves [ i ] };
        m_moves [ i ] = m_moves [ m_size ];
        return v;
    }

    [[ maybe_unused ]] Moves & operator = ( const Moves & rhs_ ) noexcept {
        std::memcpy ( this, & rhs_, sizeof ( Moves ) );
        return * this;
    }

    void remove ( const value_type m_ ) noexcept {
        if ( m_size < 2 and m_moves [ 0 ] == m_ ) {
            m_size = 0;
            return;
        }
        const Int last { m_size - 1 };
        for ( Int i = 0; i < last; ++i ) {
            if ( m_moves [ i ] == m_ ) {
                m_moves [ i ] = m_moves [ last ];
                --m_size;
                return;
            }
        }
        if ( m_moves [ last ] == m_ ) {
            --m_size;
        }
    }

    [[ nodiscard ]] auto begin ( ) noexcept {
        return std::begin ( m_moves );
    }
    [[ nodiscard ]] auto begin ( ) const noexcept {
        return std::begin ( m_moves );
    }
    [[ nodiscard ]] auto cbegin ( ) const noexcept {
        return std::cbegin ( m_moves );
    }

    [[ nodiscard ]] auto end ( ) noexcept {
        return begin ( ) + m_size;
    }
    [[ nodiscard ]] auto end ( ) const noexcept {
        return begin ( ) + m_size;
    }
    [[ nodiscard ]] auto cend ( ) const noexcept {
        return cbegin ( ) + m_size;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Moves & m_ ) noexcept {
        for ( Int i = 0; i < m_.m_size; ++i ) {
            out_ << ( int ) m_.m_moves [ i ].value << L' ';
        }
        out_ << L'\n';
        return out_;
    }

    private:

    friend class cereal::access;

    template < class Archive >
    void serialize ( Archive & ar_ ) noexcept {
        ar_ ( m_size );
        ar_ ( cereal::binary_data ( & m_moves, m_size * sizeof ( T ) ) );
    }
};
