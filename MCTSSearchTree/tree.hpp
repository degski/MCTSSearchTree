
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
#include <cstdlib>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include "types.hpp"
#include "padded.hpp"
#include "splitmix.hpp"
#include "mcts_emu.hpp"
#include "moves.hpp"
#include "link.hpp"
#include "path.hpp"


#define ARCID_INVALID_VALUE ( INT_MIN )

struct ArcID {

    Int value;

    static const ArcID invalid;

    explicit ArcID ( ) noexcept :
        value { ARCID_INVALID_VALUE } {
    }
    explicit ArcID ( Int && v_ ) noexcept :
        value { std::move ( v_ ) } {
    }
    explicit ArcID ( const Int & v_ ) noexcept :
        value { v_ } {
    }
    explicit ArcID ( const std::size_t & v_ ) noexcept :
        value { static_cast<Int> ( v_ ) } {
    }

    [[ nodiscard ]] constexpr const Int operator ( ) ( ) const noexcept {
        return value;
    }

    [[ nodiscard ]] bool operator == ( const ArcID rhs_ ) const noexcept {
        return value == rhs_.value;
    }
    [[ nodiscard ]] bool operator != ( const ArcID rhs_ ) const noexcept {
        return value != rhs_.value;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const ArcID id_ ) noexcept {
        if ( ARCID_INVALID_VALUE == id_.value ) {
            out_ << L'*';
        }
        else {
            out_ << static_cast< std::uint64_t> ( id_.value );
        }
        return out_;
    }

    private:

    friend class cereal::access;

    template < class Archive >
    inline void serialize ( Archive & ar_ ) {
        ar_ ( value );
    }
};

const ArcID ArcID::invalid { ARCID_INVALID_VALUE };


#define NODEID_INVALID_VALUE ( INT_MIN + 1 )

struct NodeID {

    Int value;

    static const NodeID invalid;

    explicit NodeID ( ) noexcept :
        value { NODEID_INVALID_VALUE } {
    }
    explicit NodeID ( Int && v_ ) noexcept :
        value { std::move ( v_ ) } {
    }
    explicit NodeID ( const Int & v_ ) noexcept :
        value { v_ } {
    }
    explicit NodeID ( const std::size_t & v_ ) noexcept :
        value { static_cast<Int> ( v_ ) } {
    }

    [[ nodiscard ]] constexpr const Int operator ( ) ( ) const noexcept {
        return value;
    }

    [[ nodiscard ]] bool operator == ( const NodeID rhs_ ) const noexcept {
        return value == rhs_.value;
    }
    [[ nodiscard ]] bool operator != ( const NodeID rhs_ ) const noexcept {
        return value != rhs_.value;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const NodeID id_ ) noexcept {
        if ( NODEID_INVALID_VALUE == id_.value ) {
            out_ << L'*';
        }
        else {
            out_ << static_cast<std::uint64_t> ( id_.value );
        }
        return out_;
    }

    private:

    friend class cereal::access;

    template < class Archive >
    inline void serialize ( Archive & ar_ ) {
        ar_ ( value );
    }
};

const NodeID NodeID::invalid { NODEID_INVALID_VALUE };


template<typename DataType>
struct Arc {

    NodeID source, target;
    ArcID next_in, next_out;

    using type = ArcID;
    using data_type = DataType;

    template<typename ... Args>
    Arc ( NodeID && s_, NodeID && t_, Args && ... args_ ) noexcept :
        source { std::move ( s_ ) },
        target { std::move ( t_ ) },
        data { std::forward<Args> ( args_ ) ... } {
    }
    template<typename ... Args>
    Arc ( const NodeID s_, const NodeID t_, Args && ... args_ ) noexcept :
        source { s_ },
        target { t_ },
        data { std::forward<Args> ( args_ ) ... } {
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Arc a_ ) noexcept {
        out_ << L'<' << a_.source << L' ' << a_.target << L' ' << a_.next_in << L' ' << a_.next_out << L'>';
        return out_;
    }

    protected:

    template<typename ArcData, typename NodeData>
    friend class SearchTree;

    DataType data;

    private:

    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( source, target, next_in, next_out, data );
    }
};

template<typename DataType>
struct Node {

    ArcID head_in, tail_in, head_out, tail_out;
    Int in_size = 0, out_size = 0;

    using type = NodeID;
    using data_type = DataType;

    template<typename ... Args>
    Node ( Args && ... args_ ) noexcept :
        data { std::forward<Args> ( args_ ) ... } {
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Node n_ ) noexcept {
        out_ << L'<' << n_.head_in << L' ' << n_.tail_in << L' ' << n_.head_out << L' ' << n_.tail_out <<  L' ' << n_.in_size << L' ' << n_.out_size << L'>';
        return out_;
    }

    protected:

    template<typename ArcData, typename NodeData>
    friend class SearchTree;

    DataType data;

    private:

    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( head_in, tail_in, head_out, tail_out, in_size, out_size, data );
    }
};


template<typename ArcData, typename NodeData>
class SearchTree {

    public:

    //using Arc = Padded<Arc<ArcData>>;
    //using Node = Padded<Node<NodeData>>;

    using Arc = Arc<ArcData>;
    using Node = Node<NodeData>;

    template<typename ... Args>
    SearchTree ( Args && ... args_ ) :
        root_arc { 0 },
        root_node { 0 } {
        m_nodes.emplace_back ( std::forward<Args> ( args_ ) ... );
        m_arcs.emplace_back ( NodeID::invalid, root_node );
        m_nodes [ root_node.value ].head_in = m_nodes [ root_node.value ].tail_in = root_arc;
        m_nodes [ root_node.value ].in_size = 1, m_nodes [ root_node.value ].out_size = 0;
    }

    template<typename ... Args>
    [ [ nodiscard ] ] ArcID addArc ( const NodeID source_, const NodeID target_, Args && ... args_ ) noexcept {
        const ArcID arc { m_arcs.size ( ) };
        m_arcs.emplace_back ( source_, target_, std::forward<Args> ( args_ ) ... );
        if ( m_nodes [ source_.value ].head_out == ArcID::invalid ) {
            m_nodes [ source_.value ].tail_out = m_nodes [ source_.value ].head_out = arc;
        }
        else {
            m_nodes [ source_.value ].tail_out = m_arcs [ m_nodes [ source_.value ].tail_out.value ].next_out = arc;
        }
        ++m_nodes [ source_.value ].out_size;
        if ( m_nodes [ target_.value ].head_in == ArcID::invalid ) {
            m_nodes [ target_.value ].tail_in = m_nodes [ target_.value ].head_in = arc;
        }
        else {
            m_nodes [ target_.value ].tail_in = m_arcs [ m_nodes [ target_.value ].tail_in.value ].next_in = arc;
        }
        ++m_nodes [ target_.value ].in_size;
        return arc;
    }

    template<typename ... Args>
    [ [ nodiscard ] ] NodeID addNode ( Args && ... args_ ) noexcept {
        const NodeID node { m_nodes.size ( ) };
        m_nodes.emplace_back ( std::forward<Args> ( args_ ) ... );
        return node;
    }

    class in_iterator : public std::iterator<std::forward_iterator_tag, ArcID, Int, const typename SearchTree::Arc*, const typename SearchTree::Arc&> {

        const SearchTree & g;
        ArcID arc;

        public:

        in_iterator ( const SearchTree & g_, const NodeID n_ ) noexcept :
            g { g_ },
            arc { g.m_nodes [ n_.value ].head_in } {
        }

        [[ maybe_unused ]] in_iterator & operator ++ ( ) noexcept {
            if ( ArcID::invalid != arc ) {
                arc = g.m_arcs [ arc.value ].next_in;
            }
            return *this;
        }

        [[ nodiscard ]] bool operator == ( const in_iterator & rhs_ ) const noexcept {
            return arc == rhs_.arc;
        }
        [[ nodiscard ]] bool operator != ( const in_iterator & rhs_ ) const noexcept {
            return arc != rhs_.arc;
        }

        [[ nodiscard ]] bool operator == ( const ArcID rhs_ ) const noexcept {
            return arc == rhs_;
        }
        [[ nodiscard ]] bool operator != ( const ArcID rhs_ ) const noexcept {
            return arc != rhs_;
        }

        [[ nodiscard ]] auto operator * ( ) noexcept {
            return g.m_arcs [ arc.value ];
        }
        [[ nodiscard ]] const auto operator * ( ) const noexcept {
            return g.m_arcs [ arc.value ];
        }

        [[ nodiscard ]] auto operator -> ( ) noexcept {
            return &g.m_arcs [ arc.value ];
        }
        [[ nodiscard ]] const auto operator -> ( ) const noexcept {
            return &g.m_arcs [ arc.value ];
        }
    };

    using const_in_iterator = const in_iterator;

    class out_iterator : public std::iterator<std::forward_iterator_tag, ArcID, Int, const typename SearchTree::Arc*, const typename SearchTree::Arc&> {

        const SearchTree & g;
        ArcID arc;

        public:

        out_iterator ( const SearchTree & g_, const NodeID n_ ) noexcept :
            g { g_ },
            arc { g.m_nodes [ n_.value ].head_out } {
        }

        [[ maybe_unused ]] out_iterator & operator ++ ( ) noexcept {
            if ( ArcID::invalid != arc ) {
                arc = g.m_arcs [ arc.value ].next_out;
            }
            return *this;
        }

        [[ nodiscard ]] bool operator == ( const out_iterator & rhs_ ) const noexcept {
            return arc == rhs_.arc;
        }
        [[ nodiscard ]] bool operator != ( const out_iterator & rhs_ ) const noexcept {
            return arc != rhs_.arc;
        }

        [[ nodiscard ]] bool operator == ( const ArcID rhs_ ) const noexcept {
            return arc == rhs_;
        }
        [[ nodiscard ]] bool operator != ( const ArcID rhs_ ) const noexcept {
            return arc != rhs_;
        }

        [[ nodiscard ]] auto operator * ( ) noexcept {
            return g.m_arcs [ arc.value ];
        }
        [[ nodiscard ]] const auto operator * ( ) const noexcept {
            return g.m_arcs [ arc.value ];
        }

        [[ nodiscard ]] auto operator -> ( ) noexcept {
            return & g.m_arcs [ arc.value ];
        }
        [[ nodiscard ]] const auto operator -> ( ) const noexcept {
            return & g.m_arcs [ arc.value ];
        }
    };

    using const_out_iterator = const out_iterator;

    [[ nodiscard ]] Int inArcNum ( const NodeID n_ ) const noexcept {
        return m_nodes [ n_.value ].in_size;
    }
    [[ nodiscard ]] Int outArcNum ( const NodeID n_ ) const noexcept {
        return m_nodes [ n_.value ].out_size;
    }

    [[ nodiscard ]] bool hasInArc ( const NodeID n_ ) const noexcept {
        return m_nodes [ n_.value ].in_size;
    }
    [[ nodiscard ]] bool hasOutArc ( const NodeID n_ ) const noexcept {
        return m_nodes [ n_.value ].out_size;
    }

    [[ nodiscard ]] in_iterator beginIn ( const NodeID node_ ) noexcept {
        return in_iterator { *this, node_ };
    }
    [[ nodiscard ]] const_in_iterator beginIn ( const NodeID node_ ) const noexcept {
        return const_in_iterator { *this, node_ };
    }
    [[ nodiscard ]] const_in_iterator cbeginIn ( const NodeID node_ ) const noexcept {
        return const_in_iterator { *this, node_ };
    }

    [[ nodiscard ]] out_iterator beginOut ( const NodeID node_ ) noexcept {
        return out_iterator { *this, node_ };
    }
    [[ nodiscard ]] out_iterator beginOut ( const NodeID node_ ) const noexcept {
        return const_out_iterator { *this, node_ };
    }
    [[ nodiscard ]] const_out_iterator cbeginOut ( const NodeID node_ ) const noexcept {
        return const_out_iterator { *this, node_ };
    }

    [[ nodiscard ]] ArcData & data ( const ArcID arc_ ) noexcept {
        return m_arcs [ arc_.value ].data;
    }
    [[ nodiscard ]] const ArcData & data ( const ArcID arc_ ) const noexcept {
        return m_arcs [ arc_.value ].data;
    }
    [[ nodiscard ]] NodeData & data ( const NodeID node_ ) noexcept {
        return m_nodes [ node_.value ].data;
    }
    [[ nodiscard ]] const NodeData & data ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].data;
    }

    [[ nodiscard ]] ArcData & operator [ ] ( const ArcID arc_ ) noexcept {
        return m_arcs [ arc_.value ].data;
    }
    [[ nodiscard ]] const ArcData & operator [ ] ( const ArcID arc_ ) const noexcept {
        return m_arcs [ arc_.value ].data;
    }
    [[ nodiscard ]] NodeData & operator [ ] ( const NodeID node_ ) noexcept {
        return m_nodes [ node_.value ].data;
    }
    [[ nodiscard ]] const NodeData & operator [ ] ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].data;
    }

    [[ nodiscard ]] Int arcNum ( ) const noexcept {
        return static_cast< Int > ( m_arcs.size ( ) );
    }
    [[ nodiscard ]] Int nodeNum ( ) const noexcept {
        return static_cast< Int > ( m_nodes.size ( ) );
    }

    // private:

    vector_container<Arc> m_arcs;
    vector_container<Node> m_nodes;

    public:

    ArcID root_arc;
    NodeID root_node;
};
