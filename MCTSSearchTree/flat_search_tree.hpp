
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

#include <iterator>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include "types.hpp"
#include "padded.hpp"
#include "transition.hpp"
#include "path.hpp"

namespace fst {

template<typename ArcData, typename NodeData>
class SearchTree;

namespace detail {

#define ARCID_INVALID_VALUE ( 0 )

struct ArcID {

    Int value;

    static const ArcID invalid;

    constexpr explicit ArcID ( ) noexcept :
        value { ARCID_INVALID_VALUE } { }
    explicit ArcID ( Int && v_ ) noexcept :
        value { std::move ( v_ ) } { }
    explicit ArcID ( const Int & v_ ) noexcept :
        value { v_ } { }
    explicit ArcID ( const std::size_t v_ ) noexcept :
        value { static_cast<Int> ( v_ ) } { }

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
            out_ << static_cast< std::uint64_t > ( id_.value );
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

const ArcID ArcID::invalid { };


#define NODEID_INVALID_VALUE ( 0 )

struct NodeID {

    Int value;

    static const NodeID invalid;

    constexpr explicit NodeID ( ) noexcept :
        value { NODEID_INVALID_VALUE } { }
    explicit NodeID ( Int && v_ ) noexcept :
        value { std::move ( v_ ) } { }
    explicit NodeID ( const Int & v_ ) noexcept :
        value { v_ } { }
    explicit NodeID ( const std::size_t v_ ) noexcept :
        value { static_cast<Int> ( v_ ) } { }

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

    template<class Archive>
    inline void serialize ( Archive & ar_ ) {
        ar_ ( value );
    }
};

const NodeID NodeID::invalid { };


template<typename DataType>
struct Arc {

    NodeID source, target;
    ArcID next_in, next_out;

    using type = ArcID;
    using data_type = DataType;

    constexpr Arc ( ) noexcept {
    }
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
    friend class fst::SearchTree;

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

    constexpr Node ( ) noexcept {
    }
    template<typename ... Args>
    Node ( Args && ... args_ ) noexcept :
        data { std::forward<Args> ( args_ ) ... } {
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, const Node node_ ) noexcept {
        out_ << L'<' << node_.head_in << L' ' << node_.tail_in << L' ' << node_.head_out << L' ' << node_.tail_out << L' ' << node_.in_size << L' ' << node_.out_size << L'>';
        return out_;
    }

    protected:

    template<typename ArcData, typename NodeData>
    friend class fst::SearchTree;

    DataType data;

    private:

    friend class cereal::access;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( head_in, tail_in, head_out, tail_out, in_size, out_size, data );
    }
};

} // namespace detail.


template<typename ArcData, typename NodeData>
class SearchTree {

    public:

    using ArcID = detail::ArcID;
    using NodeID = detail::NodeID;
    using Arc = detail::Arc<ArcData>;
    using Arcs = tagged_vector<Arc>;
    using Node = detail::Node<NodeData>;
    using Nodes = tagged_vector<Node>;
    using Transistion = Transistion<SearchTree>;
    using OptionalLink = OptionalLink<SearchTree>;
    using Path = Path<SearchTree>;

    template<typename ... Args>
    SearchTree ( Args && ... args_ ) :
        root_arc { 1 },
        root_node { 1 },
        m_arcs { { }, { NodeID::invalid, root_node } },
        m_nodes { { }, { std::forward<Args> ( args_ ) ... } } {
        m_nodes [ root_node.value ].head_in = m_nodes [ root_node.value ].tail_in = root_arc;
        m_nodes [ root_node.value ].in_size = 1, m_nodes [ root_node.value ].out_size = 0;
    }

    template<typename ... Args>
    [[ maybe_unused ]] ArcID addArc ( const NodeID source_, const NodeID target_, Args && ... args_ ) noexcept {
        const ArcID id { m_arcs.size ( ) };
        m_arcs.emplace_back ( source_, target_, std::forward<Args> ( args_ ) ... );
        if ( ArcID::invalid == m_nodes [ source_.value ].head_out ) {
            m_nodes [ source_.value ].tail_out = m_nodes [ source_.value ].head_out = id;
        }
        else {
            m_nodes [ source_.value ].tail_out = m_arcs [ m_nodes [ source_.value ].tail_out.value ].next_out = id;
        }
        ++m_nodes [ source_.value ].out_size;
        if ( ArcID::invalid == m_nodes [ target_.value ].head_in ) {
            m_nodes [ target_.value ].tail_in = m_nodes [ target_.value ].head_in = id;
        }
        else {
            m_nodes [ target_.value ].tail_in = m_arcs [ m_nodes [ target_.value ].tail_in.value ].next_in = id;
        }
        ++m_nodes [ target_.value ].in_size;
        return id;
    }

    template<typename ... Args>
    [[ maybe_unused ]] NodeID addNode ( Args && ... args_ ) noexcept {
        const NodeID id { m_nodes.size ( ) };
        m_nodes.emplace_back ( std::forward<Args> ( args_ ) ... );
        return id;
    }

    class in_iterator {

        SearchTree & m_st;
        ArcID m_id;

        public:

        using difference_type = typename Arcs::difference_type;
        using value_type = typename Arcs::value_type;
        using reference = typename Arcs::reference;
        using pointer = typename Arcs::pointer;
        using const_reference = typename Arcs::const_reference;
        using const_pointer = typename Arcs::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        in_iterator ( SearchTree & tree_, const NodeID node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_in } {
        }

        [[ nodiscard ]] const bool is_valid ( ) const noexcept {
            return ArcID::invalid != m_id;
        }

        [[ maybe_unused ]] in_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_in;
            return *this;
        }

        [[ nodiscard ]] reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] const ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    class const_in_iterator {

        const SearchTree & m_st;
        ArcID m_id;

        public:

        using difference_type = typename Arcs::difference_type;
        using value_type = typename Arcs::value_type;
        using reference = typename Arcs::reference;
        using pointer = typename Arcs::pointer;
        using const_reference = typename Arcs::const_reference;
        using const_pointer = typename Arcs::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        const_in_iterator ( const SearchTree & tree_, const NodeID node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_in } {
        }

        [[ nodiscard ]] const bool is_valid ( ) const noexcept {
            return ArcID::invalid != m_id;
        }

        [[ maybe_unused ]] const_in_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_in;
            return *this;
        }

        [[ nodiscard ]] const_reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] const_pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] const ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    class out_iterator {

        SearchTree & m_st;
        ArcID m_id;

        public:

        using difference_type = typename Arcs::difference_type;
        using value_type = typename Arcs::value_type;
        using reference = typename Arcs::reference;
        using pointer = typename Arcs::pointer;
        using const_reference = typename Arcs::const_reference;
        using const_pointer = typename Arcs::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        out_iterator ( SearchTree & tree_, const NodeID node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_out } {
        }

        [[ nodiscard ]] const bool is_valid ( ) const noexcept {
            return ArcID::invalid != m_id;
        }

        [[ maybe_unused ]] out_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_out;
            return *this;
        }

        [[ nodiscard ]] reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] const ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    class const_out_iterator {

        const SearchTree & m_st;
        ArcID m_id;

        public:

        using difference_type = typename Arcs::difference_type;
        using value_type = typename Arcs::value_type;
        using reference = typename Arcs::reference;
        using pointer = typename Arcs::pointer;
        using const_reference = typename Arcs::const_reference;
        using const_pointer = typename Arcs::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        const_out_iterator ( const SearchTree & tree_, const NodeID node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_out } {
        }

        [[ nodiscard ]] const bool is_valid ( ) const noexcept {
            return ArcID::invalid != m_id;
        }

        [[ maybe_unused ]] const_out_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_out;
            return *this;
        }

        [[ nodiscard ]] const_reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] const_pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] const ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    [[ nodiscard ]] Transistion link ( const ArcID arc_ ) const noexcept {
        return { arc_, m_arcs [ arc_.value ].target };
    }
    [[ nodiscard ]] OptionalLink link ( const NodeID source_, const NodeID target_ ) const noexcept {
        for ( const_in_iterator it = cbeginIn ( target_ ); it.is_valid ( ); ++it ) {
            if ( source_ == it->source ) {
                return { it.id ( ), target_ };
            }
        }
        return { };
    }
    template<typename It>
    [[ nodiscard ]] Transistion link ( const It & it_ ) const noexcept {
        return { it_.id ( ), it_->target };
    }

    [[ nodiscard ]] const bool isLeaf ( const NodeID node_ ) const noexcept {
        return not ( m_nodes [ node_.value ].out_size );
    }
    [[ nodiscard ]] const bool isInternal ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].out_size;
    }

    [[ nodiscard ]] const Int inArcNum ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].in_size;
    }
    [[ nodiscard ]] const Int outArcNum ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].out_size;
    }

    [[ nodiscard ]] const bool hasInArc ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].in_size;
    }
    [[ nodiscard ]] const bool hasOutArc ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ].out_size;
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
    [[ nodiscard ]] const_out_iterator beginOut ( const NodeID node_ ) const noexcept {
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


    [[ nodiscard ]] Arc & operator [ ] ( const ArcID arc_ ) noexcept {
        return m_arcs [ arc_.value ];
    }
    [[ nodiscard ]] const Arc & operator [ ] ( const ArcID arc_ ) const noexcept {
        return m_arcs [ arc_.value ];
    }
    [[ nodiscard ]] Node & operator [ ] ( const NodeID node_ ) noexcept {
        return m_nodes [ node_.value ];
    }
    [[ nodiscard ]] const Node & operator [ ] ( const NodeID node_ ) const noexcept {
        return m_nodes [ node_.value ];
    }


    [[ nodiscard ]] const Int arcNum ( ) const noexcept {
        return static_cast< Int > ( m_arcs.size ( ) ) - 1;
    }
    [[ nodiscard ]] const Int nodeNum ( ) const noexcept {
        return static_cast< Int > ( m_nodes.size ( ) ) - 1;
    }

    ArcID root_arc;
    NodeID root_node;

    private:

    Arcs m_arcs;
    Nodes m_nodes;
};

}
