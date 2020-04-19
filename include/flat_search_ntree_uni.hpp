
// MIT License
//
// Copyright (c) 2020 degski
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
#include <cstdlib>

#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <vector>

#include <boost/container/deque.hpp>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include <sax/vm_backed.hpp>

#include "types.hpp"

namespace fsntu {

template<typename NodeData>
class SearchTree;

namespace detail {

#define NODEID_INVALID_VALUE ( 0 )

struct NodeID {

    Int value;

    static constexpr NodeID invalid ( ) noexcept { return NodeID{ }; }

    constexpr explicit NodeID ( ) noexcept : value{ NODEID_INVALID_VALUE } {}
    explicit NodeID ( Int && v_ ) noexcept : value{ std::move ( v_ ) } {}
    explicit NodeID ( Int const & v_ ) noexcept : value{ v_ } {}
    explicit NodeID ( std::size_t const v_ ) noexcept : value{ static_cast<Int> ( v_ ) } {}

    [[nodiscard]] constexpr Int operator( ) ( ) const noexcept { return value; }

    [[nodiscard]] bool operator== ( NodeID const rhs_ ) const noexcept { return value == rhs_.value; }
    [[nodiscard]] bool operator!= ( NodeID const rhs_ ) const noexcept { return value != rhs_.value; }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, NodeID const id_ ) noexcept {
        if ( NODEID_INVALID_VALUE == id_.value ) {
            if constexpr ( std::is_same<typename Stream::char_type, wchar_t>::value ) {
                out_ << L'*';
            }
            else {
                out_ << '*';
            }
        }
        else {
            out_ << static_cast<std::uint64_t> ( id_.value );
        }
        return out_;
    }

    private:
    friend class cereal::access;

    template<class Archive>
    inline void serialize ( Archive & ar_ ) {
        ar_ ( value );
    }
};

template<typename DataType>
struct Node { // 16

    NodeID up, prev, tail; // 12
    Int size = 0;          // 4

    using type      = NodeID;
    using data_type = DataType;

    constexpr Node ( ) noexcept {}
    template<typename... Args>
    Node ( Args &&... args_ ) noexcept : data{ std::forward<Args> ( args_ )... } {}

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Node const node_ ) noexcept {
        if constexpr ( std::is_same<typename Stream::char_type, wchar_t>::value ) {
            out_ << L'<' << node_.up << L' ' << node_.prev << L' ' << node_.tail << L' ' << node_.size << L'>';
        }
        else {
            out_ << '<' << node_.up << ' ' << node_.prev << ' ' << node_.tail << ' ' << node_.size << '>';
        }
        return out_;
    }

    protected:
    template<typename NodeData>
    friend class ::fsnt::SearchTree;

    DataType data;

    private:
    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( up, prev, tail, size, data );
    }
};

} // namespace detail.

template<typename NodeData>
class SearchTree {

    public:
    using NodeID = detail::NodeID;
    using Node   = detail::Node<NodeData>;
    using Nodes  = std::vector<Node>;
    // using Nodes = sax::vm_vector<Type, Int, 1'000'000>;

    using difference_type = typename Nodes::difference_type;
    using value_type      = typename Nodes::value_type;
    using reference       = typename Nodes::reference;
    using pointer         = typename Nodes::pointer;
    using iterator        = typename Nodes::iterator;
    using const_reference = typename Nodes::const_reference;
    using const_pointer   = typename Nodes::const_pointer;
    using const_iterator  = typename Nodes::const_iterator;

    template<typename... Args>
    SearchTree ( Args &&... args_ ) : root_node{ 1 }, m_nodes{ Node{ }, Node{ std::forward<Args> ( args_ )... } } {}

    template<typename... Args>
    [[maybe_unused]] NodeID addNode ( NodeID const source_, Args &&... args_ ) noexcept {
        NodeID id{ m_nodes.size ( ) };
        m_nodes.emplace_back ( std::forward<Args> ( args_ )... );
        m_nodes.back ( ).up = source_;
        if ( NodeID::invalid ( ) != m_nodes[ source_.value ].tail )
            m_nodes[ id.value ].prev = m_nodes[ source_.value ].tail;
        m_nodes[ source_.value ].tail = id;
        ++m_nodes[ source_.value ].size;
        return id;
    }

    [[nodiscard]] const_iterator begin ( ) const noexcept { return m_nodes.begin ( ); }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return begin ( ); }
    [[nodiscard]] iterator begin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).begin ( ) ); }

    [[nodiscard]] const_iterator end ( ) const noexcept { return m_nodes.end ( ); }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return end ( ); }
    [[nodiscard]] iterator end ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).end ( ) ); }

    class out_iterator {

        friend class SearchTree;

        SearchTree & m_st;
        NodeID m_id;

        public:
        using difference_type   = typename Nodes::difference_type;
        using value_type        = typename Nodes::value_type;
        using reference         = typename Nodes::reference;
        using pointer           = typename Nodes::pointer;
        using const_reference   = typename Nodes::const_reference;
        using const_pointer     = typename Nodes::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        out_iterator ( SearchTree & tree_, NodeID const node_ ) noexcept :
            m_st{ tree_ }, m_id{ m_st.m_nodes[ node_.value ].tail } {}

        [[nodiscard]] bool is_valid ( ) const noexcept { return NodeID::invalid ( ) != m_id; }

        [[maybe_unused]] out_iterator & operator++ ( ) noexcept {
            m_id = m_st.m_nodes[ m_id.value ].prev;
            return *this;
        }

        [[nodiscard]] reference operator* ( ) const noexcept { return m_st.m_nodes[ m_id.value ]; }

        [[nodiscard]] pointer operator-> ( ) const noexcept { return m_st.m_nodes.data ( ) + m_id.value; }

        [[nodiscard]] NodeID id ( ) const noexcept { return m_id; }
    };

    class const_out_iterator {

        friend class SearchTree;

        SearchTree const & m_st;
        NodeID m_id;

        public:
        using difference_type   = typename Nodes::difference_type;
        using value_type        = typename Nodes::value_type;
        using reference         = typename Nodes::reference;
        using pointer           = typename Nodes::pointer;
        using const_reference   = typename Nodes::const_reference;
        using const_pointer     = typename Nodes::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        const_out_iterator ( SearchTree const & tree_, NodeID const node_ ) noexcept :
            m_st{ tree_ }, m_id{ m_st.m_nodes[ node_.value ].tail } {}

        [[nodiscard]] bool is_valid ( ) const noexcept { return NodeID::invalid ( ) != m_id; }

        [[maybe_unused]] const_out_iterator & operator++ ( ) noexcept {
            m_id = m_st.m_nodes[ m_id.value ].prev;
            return *this;
        }

        [[nodiscard]] const_reference operator* ( ) const noexcept { return m_st.m_nodes[ m_id.value ]; }

        [[nodiscard]] const_pointer operator-> ( ) const noexcept { return m_st.m_nodes.data ( ) + m_id.value; }

        [[nodiscard]] NodeID id ( ) const noexcept { return m_id; }
    };

    [[nodiscard]] bool isLeaf ( NodeID const node_ ) const noexcept { return not m_nodes[ node_.value ].size; }
    [[nodiscard]] bool isInternal ( NodeID const node_ ) const noexcept { return m_nodes[ node_.value ].size; }

    [[nodiscard]] Int arity ( NodeID const node_ ) const noexcept { return m_nodes[ node_.value ].size; }

    [[nodiscard]] NodeData & operator[] ( NodeID const node_ ) noexcept { return m_nodes[ node_.value ].data; }
    [[nodiscard]] NodeData const & operator[] ( NodeID const node_ ) const noexcept { return m_nodes[ node_.value ].data; }

    // The number of valid nodes. This is not the same as the size of
    // the nodes-vector, which allows for some additional admin elements,
    // use nodesSize ( ) instead.
    [[nodiscard]] Int nodeNum ( ) const noexcept { return static_cast<Int> ( m_nodes.size ( ) ) - 1; }

    // The size of the nodes-vector (allows for some admin elements).
    [[nodiscard]] std::size_t nodesSize ( ) const noexcept { return m_nodes.size ( ); }

    // Data members.

    NodeID root_node;

    private:
    Nodes m_nodes;
};

} // namespace fsntu
