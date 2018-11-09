
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

#include <random>
#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "splitmix.hpp"


extern splitmix64 rng;


template<typename T, std::int32_t S>
class Moves {

    typedef T moves_t [ S ]; // Typedeffing a c-array...

    uindex_t m_size = 0;
    moves_t m_moves;

public:

    using value_type = T;

    inline void clear ( ) noexcept {

        m_size = 0;
    }

    uindex_t size ( ) const noexcept {

        return m_size;
    }

    uindex_t& size ( ) noexcept {

        return m_size;
    }

    uindex_t capacity ( ) const noexcept {

        return S;
    }

    bool empty ( ) const noexcept {

        return !m_size;
    }

    value_type at ( const uindex_t i_ ) const noexcept {

        assert ( i_ >= 0 );
        assert ( i_ < size ( ) );

        return m_moves [ i_ ];
    }

    value_type front ( ) const noexcept {

        return m_moves [ 0 ];
    }

    void push_back ( const value_type m_ ) noexcept {

        m_moves [ m_size++ ] = m_;
    }

    void emplace_back ( value_type && m_ ) noexcept {

        m_moves [ m_size++ ] = std::move ( m_ );
    }

    value_type random ( ) const noexcept {

        return m_moves [ std::uniform_int_distribution<std::ptrdiff_t> ( 0, m_size - 1 ) ( rng ) ];
    }

    bool find ( const value_type m_ ) const noexcept {
        for ( uindex_t i = 0; i < m_size; ++i ) {
            if ( m_moves [ i ] == m_ ) {
                return true;
            }
        }
        return false;
    }

    value_type draw ( ) noexcept {
        const uindex_t i = std::uniform_int_distribution < uindex_t > ( 0, m_size - 1 ) ( rng );
        const value_type v = m_moves [ i ];
        m_moves [ i ] = m_moves [ --m_size ];
        return v;
    }

    Moves & operator = ( const Moves & rhs_ ) noexcept {
        memcpy ( this, & rhs_, sizeof ( Moves ) );
        return * this;
    }

    void remove ( const value_type m_ ) noexcept {
        if ( m_size < 2 and m_moves [ 0 ] == m_ ) {
            m_size = 0;
            return;
        }
        const uindex_t last = m_size - 1;

        for ( uindex_t i = 0; i < last; ++i ) {

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

    void print ( ) const noexcept {
        for ( uindex_t i = 0; i < m_size; ++i ) {

            // putchar ( ( int ) m_moves [ i ].m_loc + 48 ); putchar ( ' ' );

            // putchar ( ( int ) m_moves [ i ] + 48 ); putchar ( ' ' );

            std::cout << ( int ) m_moves [ i ] << ' ';
        }
        putchar ( '\n' );
    }

    template < class Archive >
    inline void serialize ( Archive & ar_ ) noexcept {
        ar_ ( m_size );
        ar_ ( cereal::binary_data ( & m_moves, m_size * sizeof ( T ) ) );
    }

    inline auto begin ( ) {
        return std::begin ( m_moves );
    }
    inline const auto cbegin ( ) const {
        return std::cbegin ( m_moves );
    }
    inline auto end ( ) {
        return std::end ( m_moves );
    }
    inline const auto cend ( ) const {
        return std::cend ( m_moves );
    }
};
