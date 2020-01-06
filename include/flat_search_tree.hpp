
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

#include "types.hpp"
#include "link.hpp"
#include "path.hpp"


namespace fst {

template<typename ArcData, typename NodeData>
class SearchTree;

namespace detail {

#define ARCID_INVALID_VALUE ( 0 )

struct ArcID {

    Int value;

    static constexpr ArcID invalid ( ) noexcept {
        return ArcID { };
    }

    constexpr explicit ArcID ( ) noexcept :
        value { ARCID_INVALID_VALUE } { }
    explicit ArcID ( Int && v_ ) noexcept :
        value { std::move ( v_ ) } { }
    explicit ArcID ( Int const & v_ ) noexcept :
        value { v_ } { }
    explicit ArcID ( std::size_t const v_ ) noexcept :
        value { static_cast<Int> ( v_ ) } { }

    [[ nodiscard ]] constexpr Int operator ( ) ( ) const noexcept {
        return value;
    }

    [[ nodiscard ]] bool operator == ( ArcID const rhs_ ) const noexcept {
        return value == rhs_.value;
    }
    [[ nodiscard ]] bool operator != ( ArcID const rhs_ ) const noexcept {
        return value != rhs_.value;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, ArcID const id_ ) noexcept {
        if ( ARCID_INVALID_VALUE == id_.value ) {
            if constexpr ( std::is_same<typename Stream::char_type, wchar_t>::value ) {
                out_ << L'*';
            }
            else {
                out_ << '*';
            }
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


#define NODEID_INVALID_VALUE ( 0 )

struct NodeID {

    Int value;

    static constexpr NodeID invalid ( ) noexcept {
        return NodeID { };
    }

    constexpr explicit NodeID ( ) noexcept :
        value { NODEID_INVALID_VALUE } { }
    explicit NodeID ( Int && v_ ) noexcept :
        value { std::move ( v_ ) } { }
    explicit NodeID ( Int const & v_ ) noexcept :
        value { v_ } { }
    explicit NodeID ( std::size_t const v_ ) noexcept :
        value { static_cast<Int> ( v_ ) } { }

    [[ nodiscard ]] constexpr Int operator ( ) ( ) const noexcept {
        return value;
    }

    [[ nodiscard ]] bool operator == ( NodeID const rhs_ ) const noexcept {
        return value == rhs_.value;
    }
    [[ nodiscard ]] bool operator != ( NodeID const rhs_ ) const noexcept {
        return value != rhs_.value;
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, NodeID const id_ ) noexcept {
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
    Arc ( NodeID const s_, NodeID const t_, Args && ... args_ ) noexcept :
        source { s_ },
        target { t_ },
        data { std::forward<Args> ( args_ ) ... } {
    }

    template<typename Stream>
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, Arc const a_ ) noexcept {
        if constexpr ( std::is_same<typename Stream::char_type, wchar_t>::value ) {
            out_ << L'<' << a_.source << L' ' << a_.target << L' ' << a_.next_in << L' ' << a_.next_out << L'>';
        }
        else {
            out_ << '<' << a_.source << ' ' << a_.target << ' ' << a_.next_in << ' ' << a_.next_out << '>';
        }
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
    [[ maybe_unused ]] friend Stream & operator << ( Stream & out_, Node const node_ ) noexcept {
        if constexpr ( std::is_same<typename Stream::char_type, wchar_t>::value ) {
            out_ << L'<' << node_.head_in << L' ' << node_.tail_in << L' ' << node_.head_out << L' ' << node_.tail_out << L' ' << node_.in_size << L' ' << node_.out_size << L'>';
        }
        else {
            out_ << '<' << node_.head_in << ' ' << node_.tail_in << ' ' << node_.head_out << ' ' << node_.tail_out << ' ' << node_.in_size << ' ' << node_.out_size << '>';
        }
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
    using Arcs = std::vector<Arc>;
    using Node = detail::Node<NodeData>;
    using Nodes = std::vector<Node>;
    using Link = Link<SearchTree>;
    using OptionalLink = OptionalLink<SearchTree>;
    using Path = Path<SearchTree>;
    using Visited = std::vector<NodeID>; // New m_nodes by old_index.
    using Stack = std::vector<NodeID>;
    using Queue = boost::container::deque<NodeID>;

    template<typename ... Args>
    SearchTree ( Args && ... args_ ) :
        root_arc { 1 },
        root_node { 1 },
        m_arcs { { }, { NodeID::invalid ( ), root_node } },
        m_nodes { { }, { std::forward<Args> ( args_ ) ... } } {
        m_nodes [ root_node.value ].head_in = m_nodes [ root_node.value ].tail_in = root_arc;
        m_nodes [ root_node.value ].in_size = 1, m_nodes [ root_node.value ].out_size = 0;
    }

    template<typename ... Args>
    [[ maybe_unused ]] ArcID addArc ( NodeID const source_, NodeID const target_, Args && ... args_ ) noexcept {
        ArcID const id { m_arcs.size ( ) };
        m_arcs.emplace_back ( source_, target_, std::forward<Args> ( args_ ) ... );
        if ( ArcID::invalid ( ) == m_nodes [ source_.value ].head_out )
            m_nodes [ source_.value ].tail_out = m_nodes [ source_.value ].head_out = id;
        else
            m_nodes [ source_.value ].tail_out = m_arcs [ m_nodes [ source_.value ].tail_out.value ].next_out = id;
        ++m_nodes [ source_.value ].out_size;
        if ( ArcID::invalid ( ) == m_nodes [ target_.value ].head_in )
            m_nodes [ target_.value ].tail_in = m_nodes [ target_.value ].head_in = id;
        else
            m_nodes [ target_.value ].tail_in = m_arcs [ m_nodes [ target_.value ].tail_in.value ].next_in = id;
        ++m_nodes [ target_.value ].in_size;
        return id;
    }

    template<typename ... Args>
    [[ maybe_unused ]] NodeID addNode ( Args && ... args_ ) noexcept {
        NodeID const id { m_nodes.size ( ) };
        m_nodes.emplace_back ( std::forward<Args> ( args_ ) ... );
        return id;
    }

    class node_iterator {

        friend class SearchTree;

        typename Nodes::pointer m_ptr, m_end;

        public:

        using difference_type = typename Nodes::difference_type;
        using value_type = typename Nodes::value_type;
        using reference = typename Nodes::reference;
        using pointer = typename Nodes::pointer;
        using const_reference = typename Nodes::const_reference;
        using const_pointer = typename Nodes::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        node_iterator ( SearchTree & tree_ ) noexcept :
            m_ptr { tree_.m_nodes.data ( ) },
            m_end { m_ptr + tree_.m_nodes.size ( ) } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return m_end != m_ptr;
        }

        [[ maybe_unused ]] node_iterator & operator ++ ( ) noexcept {
            ++m_ptr;
            return * this;
        }

        [[ nodiscard ]] reference operator * ( ) const noexcept {
            return *m_ptr;
        }

        [[ nodiscard ]] pointer operator -> ( ) const noexcept {
            return m_ptr;
        }
    };

    class const_node_iterator {

        friend class SearchTree;

        typename Nodes::pointer m_ptr, m_end;

        public:

        using difference_type = typename Nodes::difference_type;
        using value_type = typename Nodes::value_type;
        using reference = typename Nodes::reference;
        using pointer = typename Nodes::pointer;
        using const_reference = typename Nodes::const_reference;
        using const_pointer = typename Nodes::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        const_node_iterator ( SearchTree const & tree_ ) noexcept :
            m_ptr { tree_.m_nodes.data ( ) },
            m_end { m_ptr + tree_.m_nodes.size ( ) } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return m_end != m_ptr;
        }

        [[ maybe_unused ]] const_node_iterator & operator ++ ( ) noexcept {
            ++m_ptr;
            return * this;
        }

        [[ nodiscard ]] const_reference operator * ( ) const noexcept {
            return *m_ptr;
        }

        [[ nodiscard ]] const_pointer operator -> ( ) const noexcept {
            return m_ptr;
        }
    };

    class iterator {

        friend class SearchTree;

        typename Arcs::pointer m_ptr, m_end;

        public:

        using difference_type = typename Arcs::difference_type;
        using value_type = typename Arcs::value_type;
        using reference = typename Arcs::reference;
        using pointer = typename Arcs::pointer;
        using const_reference = typename Arcs::const_reference;
        using const_pointer = typename Arcs::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        iterator ( SearchTree & tree_ ) noexcept :
            m_ptr { tree_.m_arcs.data ( ) },
            m_end { m_ptr + tree_.m_arcs.size ( ) } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return m_end != m_ptr;
        }

        [[ maybe_unused ]] iterator & operator ++ ( ) noexcept {
            ++m_ptr;
            return * this;
        }

        [[ nodiscard ]] reference operator * ( ) const noexcept {
            return *m_ptr;
        }

        [[ nodiscard ]] pointer operator -> ( ) const noexcept {
            return m_ptr;
        }
    };

    class const_iterator {

        friend class SearchTree;

        typename Arcs::pointer m_ptr, m_end;

        public:

        using difference_type = typename Arcs::difference_type;
        using value_type = typename Arcs::value_type;
        using reference = typename Arcs::reference;
        using pointer = typename Arcs::pointer;
        using const_reference = typename Arcs::const_reference;
        using const_pointer = typename Arcs::const_pointer;
        using iterator_category = std::forward_iterator_tag;

        const_iterator ( SearchTree const & tree_ ) noexcept :
            m_ptr { tree_.m_arcs.data ( ) },
            m_end { m_ptr + tree_.m_arcs.size ( ) } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return m_end != m_ptr;
        }

        [[ maybe_unused ]] const_iterator & operator ++ ( ) noexcept {
            ++m_ptr;
            return * this;
        }

        [[ nodiscard ]] const_reference operator * ( ) const noexcept {
            return *m_ptr;
        }

        [[ nodiscard ]] const_pointer operator -> ( ) const noexcept {
            return m_ptr;
        }
    };

    class in_iterator {

        friend class SearchTree;

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

        in_iterator ( SearchTree & tree_, NodeID const node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_in } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return ArcID::invalid ( ) != m_id;
        }

        [[ maybe_unused ]] in_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_in;
            return * this;
        }

        [[ nodiscard ]] reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    class const_in_iterator {

        friend class SearchTree;

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

        const_in_iterator ( SearchTree const & tree_, NodeID const node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_in } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return ArcID::invalid ( ) != m_id;
        }

        [[ maybe_unused ]] const_in_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_in;
            return * this;
        }

        [[ nodiscard ]] const_reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] const_pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    class out_iterator {

        friend class SearchTree;

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

        out_iterator ( SearchTree & tree_, NodeID const node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_out } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return ArcID::invalid ( ) != m_id;
        }

        [[ maybe_unused ]] out_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_out;
            return * this;
        }

        [[ nodiscard ]] reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    class const_out_iterator {

        friend class SearchTree;

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

        const_out_iterator ( SearchTree const & tree_, NodeID const node_ ) noexcept :
            m_st { tree_ },
            m_id { m_st.m_nodes [ node_.value ].head_out } {
        }

        [[ nodiscard ]] bool is_valid ( ) const noexcept {
            return ArcID::invalid ( ) != m_id;
        }

        [[ maybe_unused ]] const_out_iterator & operator ++ ( ) noexcept {
            m_id = m_st.m_arcs [ m_id.value ].next_out;
            return * this;
        }

        [[ nodiscard ]] const_reference operator * ( ) const noexcept {
            return m_st.m_arcs [ m_id.value ];
        }

        [[ nodiscard ]] const_pointer operator -> ( ) const noexcept {
            return m_st.m_arcs.data ( ) + m_id.value;
        }

        [[ nodiscard ]] ArcID id ( ) const noexcept {
            return m_id;
        }
    };

    [[ nodiscard ]] Link link ( ArcID const arc_ ) const noexcept {
        return { arc_, m_arcs [ arc_.value ].target };
    }
    [[ nodiscard ]] OptionalLink link ( NodeID const source_, NodeID const target_ ) const noexcept {
        for ( const_in_iterator it = cbeginIn ( target_ ); it.is_valid ( ); ++it )
            if ( source_ == it->source )
                return { { it.id ( ), target_ } };
        return { };
    }
    template<typename It>
    [[ nodiscard ]] Link link ( It const & it_ ) const noexcept {
        return { it_.id ( ), it_->target };
    }

    [[ nodiscard ]] bool isLeaf ( NodeID const node_ ) const noexcept {
        return not ( m_nodes [ node_.value ].out_size );
    }
    [[ nodiscard ]] bool isInternal ( NodeID const node_ ) const noexcept {
        return m_nodes [ node_.value ].out_size;
    }

    [[ nodiscard ]] Int inArcNum ( NodeID const node_ ) const noexcept {
        return m_nodes [ node_.value ].in_size;
    }
    [[ nodiscard ]] Int outArcNum ( NodeID const node_ ) const noexcept {
        return m_nodes [ node_.value ].out_size;
    }

    [[ nodiscard ]] bool hasInArc ( NodeID const node_ ) const noexcept {
        return m_nodes [ node_.value ].in_size;
    }
    [[ nodiscard ]] bool hasOutArc ( NodeID const node_ ) const noexcept {
        return m_nodes [ node_.value ].out_size;
    }

    [[ nodiscard ]] in_iterator beginIn ( NodeID const node_ ) noexcept {
        return in_iterator { * this, node_ };
    }
    [[ nodiscard ]] const_in_iterator beginIn ( NodeID const node_ ) const noexcept {
        return const_in_iterator { * this, node_ };
    }
    [[ nodiscard ]] const_in_iterator cbeginIn ( NodeID const node_ ) const noexcept {
        return const_in_iterator { * this, node_ };
    }

    [[ nodiscard ]] out_iterator beginOut ( NodeID const node_ ) noexcept {
        return out_iterator { * this, node_ };
    }
    [[ nodiscard ]] const_out_iterator beginOut ( NodeID const node_ ) const noexcept {
        return const_out_iterator { * this, node_ };
    }
    [[ nodiscard ]] const_out_iterator cbeginOut ( NodeID const node_ ) const noexcept {
        return const_out_iterator { * this, node_ };
    }

    [[ nodiscard ]] ArcData & operator [ ] ( ArcID const arc_ ) noexcept {
        return m_arcs [ arc_.value ].data;
    }
    [[ nodiscard ]] ArcData const & operator [ ] ( ArcID const arc_ ) const noexcept {
        return m_arcs [ arc_.value ].data;
    }
    [[ nodiscard ]] NodeData & operator [ ] ( NodeID const node_ ) noexcept {
        return m_nodes [ node_.value ].data;
    }
    [[ nodiscard ]] NodeData const & operator [ ] ( NodeID const node_ ) const noexcept {
        return m_nodes [ node_.value ].data;
    }

    // The number of valid arcs. This is not the same as the size of
    // the arcs-vector, which allows for some additional admin elements,
    // use arcsSize ( ) instead.
    [[ nodiscard ]] Int arcNum ( ) const noexcept {
        return static_cast<Int> ( m_arcs.size ( ) ) - 2;
    }
    // The number of valid nodes. This is not the same as the size of
    // the arcs-vector, which allows for some additional admin elements,
    // use nodesSize ( ) instead.
    [[ nodiscard ]] Int nodeNum ( ) const noexcept {
        return static_cast<Int> ( m_nodes.size ( ) ) - 1;
    }

    // The size of the arcs-vector (allows for some admin elements).
    [[ nodiscard ]] std::size_t arcsSize ( ) const noexcept {
        return m_arcs.size ( );
    }
    // The size of the nodes-vector (allows for some admin elements).
    [[ nodiscard ]] std::size_t nodesSize ( ) const noexcept {
        return m_nodes.size ( );
    }

    // Destructively construct a sub-tree out of the current tree [Depth First].
    [[ nodiscard ]] SearchTree makeSubTree ( NodeID const root_node_to_be_ ) {
        assert ( NodeID::invalid ( ) != root_node_to_be_ );
        assert ( root_node != root_node_to_be_ );
        SearchTree sub_tree { std::move ( m_nodes [ root_node_to_be_.value ].data ) };
        // The Visited-vector stores the new NodeID's indexed by old NodeID's,
        // old NodeID's not present in the new tree have a value of NodeID::invalid ( ).
        static Visited visited;
        visited.clear ( );
        visited.resize ( m_nodes.size ( ), NodeID::invalid ( ) );
        visited [ root_node_to_be_.value ] = sub_tree.root_node;
        static Stack stack;
        stack.clear ( );
        stack.push_back ( root_node_to_be_ );
        while ( stack.size ( ) ) {
            NodeID const parent = stack.back ( ); stack.pop_back ( );
            for ( ArcID a = m_nodes [ parent.value ].head_out; ArcID::invalid ( ) != a; a = m_arcs [ a.value ].next_out ) {
                NodeID const child { m_arcs [ a.value ].target };
                if ( NodeID::invalid ( ) == visited [ child.value ] ) { // Not visited yet.
                    visited [ child.value ] = sub_tree.addNode ( std::move ( m_nodes [ child.value ].data ) );
                    stack.push_back ( child );
                }
                sub_tree.addArc ( visited [ parent.value ], visited [ child.value ], std::move ( m_arcs [ a.value ].data ) );
            }
        }
        return sub_tree;
    }

    void traverseBreadthFirst ( NodeID const root_node_to_be_ = NodeID { 1 } ) { // Default is to walk the whole tree.
        assert ( NodeID::invalid ( ) != root_node_to_be_ );
        // The Visited-vector stores the new NodeID's indexed by old NodeID's,
        // old NodeID's not present in the new tree have a value of NodeID::invalid ( ).
        static std::vector<bool> visited;
        visited.clear ( );
        visited.resize ( m_nodes.size ( ), false );
        visited [ root_node_to_be_.value ] = true;
        static Queue queue;
        queue.clear ( );
        queue.push_back ( root_node_to_be_ );
        while ( queue.size ( ) ) {
            NodeID const parent = queue.front ( ); queue.pop_front ( );
            for ( ArcID a = m_nodes [ parent.value ].head_out; ArcID::invalid ( ) != a; a = m_arcs [ a.value ].next_out ) {
                NodeID child { m_arcs [ a.value ].target };
                if ( not visited [ child.value ] ) { // Not visited yet.
                    visited [ child.value ] = true;
                    queue.emplace_back ( std::move ( child ) );
                    // All nodes traversed once here.
                }
                // All arcs traversed once here [nodes are traversed (possibly) several times].
            }
        }
    }

    void traverseDepthFirst ( NodeID const root_node_to_be_ = NodeID { 1 } ) { // Default is to walk the whole tree.
        assert ( NodeID::invalid ( ) != root_node_to_be_ );
        // The Visited-vector stores the new NodeID's indexed by old NodeID's,
        // old NodeID's not present in the new tree have a value of NodeID::invalid ( ).
        static std::vector<bool> visited;
        visited.clear ( );
        visited.resize ( m_nodes.size ( ), false );
        visited [ root_node_to_be_.value ] = true;
        static Stack stack;
        stack.clear ( );
        stack.push_back ( root_node_to_be_ );
        while ( stack.size ( ) ) {
            NodeID const parent = stack.back ( ); stack.pop_back ( );
            for ( ArcID a = m_nodes [ parent.value ].head_out; ArcID::invalid ( ) != a; a = m_arcs [ a.value ].next_out ) {
                NodeID child { m_arcs [ a.value ].target };
                if ( not visited [ child.value ] ) { // Not visited yet.
                    visited [ child.value ] = true;
                    stack.emplace_back ( std::move ( child ) );
                }
            }
        }
    }

    // Topological sorting, using Kahn's alogorithm (does not traverse all arcs).
    [[ nodiscard ]] std::vector<NodeID> topologicalSort ( ) const noexcept {
        std::vector<NodeID> sorted;
        static std::vector<bool> removed_arcs;
        removed_arcs.clear ( );
        removed_arcs.resize ( m_arcs.size ( ), false );
        static Stack stack;
        stack.clear ( );
        stack.push_back ( root_node );
        while ( stack.size ( ) ) {
            sorted.push_back ( stack.back ( ) ); stack.pop_back ( );
            for ( ArcID out = m_nodes [ sorted.back ( ).value ].head_out; ArcID::invalid ( ) != out; out = m_arcs [ out.value ].next_out ) {
                removed_arcs [ out.value ] = true;
                bool has_no_in_arcs = true;
                for ( ArcID in = m_nodes [ m_arcs [ out.value ].target.value ].head_in; ArcID::invalid ( ) != in; in = m_arcs [ in.value ].next_in ) {
                    if ( not removed_arcs [ in.value ] ) {
                        has_no_in_arcs = false;
                        break;
                    }
                }
                if ( has_no_in_arcs )
                    stack.push_back ( m_arcs [ out.value ].target );
            }
        }
        return sorted;
    }

    // Data members.

    ArcID root_arc;
    NodeID root_node;

    private:

    Arcs m_arcs;
    Nodes m_nodes;
};

}
