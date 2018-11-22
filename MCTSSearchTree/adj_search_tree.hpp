
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

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

#include "pool_allocator.hpp"
#include "types.hpp"
#include "link.hpp"
#include "path.hpp"


namespace ast {

template<typename ArcData, typename NodeData>
class SearchTree {

    public:

    struct Arc;
    struct Node;

    using ArcID = Arc*;
    using NodeID = Node*;

    using InList = vector_container<ArcID>;
    using in_iterator = typename InList::iterator;
    using const_in_iterator = typename InList::const_iterator;

    using OutList = vector_container<ArcID>;
    using out_iterator = typename OutList::iterator;
    using const_out_iterator = typename OutList::const_iterator;

    using Link = Link<SearchTree>;
    using Path = Path<SearchTree>;

    struct Arc {

        friend class SearchTree<ArcData, NodeData>;

        template<typename ... Args>
        Arc ( const NodeID source_, const NodeID target_, Args && ... args_ ) :
            source { source_ },
            target { target_ },
            data { std::forward<Args> ( args_ ) ... } {
        }

        NodeID source, target;
        ArcData data;
    };

    struct Node {

        friend class SearchTree<ArcData, NodeData>;

        template<typename ... Args>
        Node ( Args && ... args_ ) :
            data { std::forward<Args> ( args_ ) ... } {
        }

        private:

        InList in_arcs;
        OutList out_arcs;

        public:

        NodeData data;
    };

    using Nodes = pa::pool_allocator<Node>;
    using Arcs = pa::pool_allocator<Arc>;


    template<typename ... Args>
    SearchTree ( Args && ... args_ ) :
        arcs_size { 0 },
        nodes_size { 0 },
        arcs { },
        nodes { },
        root_node { addNode ( std::forward<Args> ( args_ ) ... ) },
        top_node { root_node } {
    }

    void setRoot ( const NodeID node_ ) noexcept {
        root_node = node_;
        invalid_arc->target = root_node;
    }

    template<typename ... Args>
    [[ maybe_unused ]] ArcID addArc ( const NodeID source_, const NodeID target_, Args && ... args_ ) noexcept {
        ArcID arc = arcs.new_element ( source_, target_, std::forward<Args> ( args_ ) ... );
        ++arcs_size;
        outArcs ( source_ ).push_back ( arc );
        inArcs ( target_ ).push_back ( arc );
        return arc;
    }

    template<typename ... Args>
    [[ maybe_unused ]] NodeID addNode ( Args && ... args_ ) noexcept {
        NodeID node = nodes.new_element ( std::forward<Args> ( args_ ) ... );
        ++nodes_size;
        return node;
    }

    void erase ( const ArcID arc_ ) noexcept {
        --arcs_size;
        inArcs ( arc_->target ).erase ( std::remove ( std::begin ( inArcs ( arc_->target ) ), std::end ( inArcs ( arc_->target ) ), arc_ ) );
        outArcs ( arc_->source ).erase ( std::remove ( std::begin ( outArcs ( arc_->source ) ), std::end ( outArcs ( arc_->source ) ), arc_ ) );
        arcs.delete_element ( arc_ );
    }

    void erase ( const NodeID node_ ) noexcept {
        for ( auto arc : inArcs ( node_ ) ) {
            erase ( arc );
        }
        for ( auto arc : outArcs ( node_ ) ) {
            erase ( arc );
        }
        --nodes_size;
        nodes.delete_element ( node_ );
    }

    [[ nodiscard ]] Link link ( const ArcID arc_ ) const noexcept {
        return Link ( arc_, arc_->target );
    }
    [[ nodiscard ]] Link link ( const NodeID source_, const NodeID target_ ) const noexcept {
        for ( const ArcID arc : inArcs ( target_ ) ) {
            if ( source_ == arc->source ) {
                return Link ( arc, target_ );
            }
        }
        return Link ( invalid_arc, target_ );
    }
    template<typename It>
    [ [ nodiscard ] ] Link link ( const It & it_ ) const noexcept {
        return Link ( *it_, ( *it_ )->target );
    }

    [[ nodiscard ]] const bool isLeaf ( const NodeID node_ ) const noexcept {
        return node_->out_arcs.empty ( );
    }
    [[ nodiscard ]] const bool isInternal ( const NodeID node_ ) const noexcept {
        return node_->out_arcs.size ( );
    }

    [[ nodiscard ]] const std::size_t inArcNum ( const NodeID node_ ) const noexcept {
        return node_->in_arcs.size ( );
    }
    [[ nodiscard ]] const std::size_t outArcNum ( const NodeID node_ ) const noexcept {
        return node_->out_arcs.size ( );
    }

    [[ nodiscard ]] const bool hasInArc ( const NodeID node_ ) const noexcept {
        return node_->in_arcs.size ( );
    }
    [[ nodiscard ]] const bool hasOutArc ( const NodeID node_ ) const noexcept {
        return node_->out_arcs.size ( );
    }

    [[ nodiscard ]] in_iterator beginIn ( const NodeID node_ ) noexcept {
        return std::begin ( inArcs ( node_ ) );
    }
    [[ nodiscard ]] const_in_iterator beginIn ( const NodeID node_ ) const noexcept {
        return std::begin ( inArcs ( node_ ) );
    }
    [[ nodiscard ]] const_in_iterator cbeginIn ( const NodeID node_ ) const noexcept {
        return std::cbegin ( inArcs ( node_ ) );
    }
    [[ nodiscard ]] in_iterator endIn ( const NodeID node_ ) noexcept {
        return std::end ( inArcs ( node_ ) );
    }
    [[ nodiscard ]] const_in_iterator endIn ( const NodeID node_ ) const noexcept {
        return std::end ( inArcs ( node_ ) );
    }
    [[ nodiscard ]] const_in_iterator cendIn ( const NodeID node_ ) const noexcept {
        return std::cend ( inArcs ( node_ ) );
    }

    [[ nodiscard ]] out_iterator beginOut ( const NodeID node_ ) noexcept {
        return std::begin ( outArcs ( node_ ) );
    }
    [[ nodiscard ]] const_out_iterator beginOut ( const NodeID node_ ) const noexcept {
        return std::cbegin ( outArcs ( node_ ) );
    }
    [[ nodiscard ]] const_out_iterator cbeginOut ( const NodeID node_ ) const noexcept {
        return std::cbegin ( outArcs ( node_ ) );
    }
    [[ nodiscard ]] out_iterator endOut ( const NodeID node_ ) noexcept {
        return std::end ( outArcs ( node_ ) );
    }
    [[ nodiscard ]] const_out_iterator endOut ( const NodeID node_ ) const noexcept {
        return std::cend ( outArcs ( node_ ) );
    }
    [[ nodiscard ]] const_out_iterator cendOut ( const NodeID node_ ) const noexcept {
        return std::cend ( outArcs ( node_ ) );
    }

    [[ nodiscard ]] ArcData & data ( const ArcID arc_ ) noexcept {
        return arc_->data;
    }
    [[ nodiscard ]] const ArcData & data ( const ArcID arc_ ) const noexcept {
        return arc_->data;
    }
    [[ nodiscard ]] NodeData & data ( const NodeID node_ ) noexcept {
        return node_->data;
    }
    [[ nodiscard ]] const NodeData & data ( const NodeID node_ ) const noexcept {
        return node_->data;
    }

    [[ nodiscard ]] Arc & operator [ ] ( const ArcID arc_ ) noexcept {
        return * arc_;
    }
    [[ nodiscard ]] const Arc & operator [ ] ( const ArcID arc_ ) const noexcept {
        return * arc_;
    }
    [[ nodiscard ]] NodeData & operator [ ] ( const NodeID node_ ) noexcept {
        return * node_;
    }
    [[ nodiscard ]] const NodeData & operator [ ] ( const NodeID node_ ) const noexcept {
        return * node_;
    }

    [[ nodiscard ]] const std::size_t nodeNum ( ) const noexcept {
        return nodes_size;
    }
    [[ nodiscard ]] const std::size_t arcNum ( ) const noexcept {
        return arcs_size;
    }

    [[ nodiscard ]] InList & inArcs ( const NodeID node_ ) noexcept {
        return node_->in_arcs;
    }
    [[ nodiscard ]] const InList & inArcs ( const NodeID node_ ) const noexcept {
        return node_->in_arcs;
    }
    [[ nodiscard ]] OutList & outArcs ( const NodeID node_ ) noexcept {
        return node_->out_arcs;
    }
    [[ nodiscard ]] const OutList & outArcs ( const NodeID node_ ) const noexcept {
        return node_->out_arcs;
    }

    private:

    std::size_t arcs_size, nodes_size;

    Arcs arcs;
    Nodes nodes;

    public:

    NodeID root_node;
    const NodeID top_node;

    private:

    static const std::unique_ptr<Arc> smart_invalid_arc;
    static const std::unique_ptr<Node> smart_invalid_node;

    public:

    static const ArcID invalid_arc;
    static const NodeID invalid_node;
};

template<typename ArcData, typename NodeData>
const std::unique_ptr<typename SearchTree<ArcData, NodeData>::Arc> SearchTree<ArcData, NodeData>::smart_invalid_arc = std::make_unique<typename SearchTree<ArcData, NodeData>::Arc> ( smart_invalid_node.get ( ), smart_invalid_node.get ( ) );

template<typename ArcData, typename NodeData>
const std::unique_ptr<typename SearchTree<ArcData, NodeData>::Node> SearchTree<ArcData, NodeData>::smart_invalid_node = std::make_unique<typename SearchTree<ArcData, NodeData>::Node> ( );

template<typename ArcData, typename NodeData>
const typename SearchTree<ArcData, NodeData>::ArcID SearchTree<ArcData, NodeData>::invalid_arc = SearchTree<ArcData, NodeData>::smart_invalid_arc.get ( );

template<typename ArcData, typename NodeData>
const typename SearchTree<ArcData, NodeData>::NodeID SearchTree<ArcData, NodeData>::invalid_node = SearchTree<ArcData, NodeData>::smart_invalid_node.get ( );

}
