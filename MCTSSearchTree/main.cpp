
// MIT License
//
// Copyright (c) 2018, 2019, 2020 degski
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <filesystem>
#include <fstream>
#include <sax/iostream.hpp>
#include <iterator>
#include <list>
#include <map>
#include <memory_resource>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/pector.hpp>

#define NOGDI // Otherwise Arc is defined.

#include <plf/plf_nanotimer.h>

#include <sax/splitmix.hpp> // https://github.com/degski/Sax/blob/master/splitmix.hpp
#include <sax/singleton.hpp>
#include "flat_search_tree.hpp"
#include "flat_search_tree_hash.hpp"
#include "adjacency_search_tree.hpp"
#include "flat_search_ntree.hpp"
#include "flat_search_ntree_uni.hpp"
#include "link.hpp"
#include "path.hpp"
#include "mcts_emu.hpp"
#include "moves.hpp"

sax::singleton<sax::splitmix64> rng;

int main ( ) {

    using namespace fsntu;

    auto x = rng.instance ( 123u );

    using Tree = SearchTree<int>;
    using Node = typename Tree::NodeID;
    using It   = typename Tree::const_out_iterator;

    std::cout << sizeof ( Tree::Node ) << nl; // 512

    auto hash = 0x14cd518c672612a9;

    Tree t ( 1 );

    Node n2 = t.add_node ( t.root_node, 2 );

    Node n3 = t.add_node ( t.root_node, 3 );

    Node n4 = t.add_node ( t.root_node, 4 );

    Node n5 = t.add_node ( n2, 5 );

    Node n6 = t.add_node ( n2, 6 );

    Node n7 = t.add_node ( n3, 7 );

    Node n8 = t.add_node ( n4, 8 );

    Node n9 = t.add_node ( t.root_node, 9 );

    Node n10 = t.add_node ( n4, 10 );

    Node n11 = t.add_node ( n2, 11 );

    Node n12 = t.add_node ( n2, 12 );

    std::cout << t.size ( ) << nl;

    for ( It it{ t, t.root_node }; it.is_valid ( ); ++it )
        std::cout << it->data << ' ';

    std::cout << nl;

    for ( It it{ t, n2 }; it.is_valid ( ); ++it )
        std::cout << it->data << ' ';

    std::cout << nl;

    t.re_root ( n2 );

    for ( It it{ t, t.root_node }; it.is_valid ( ); ++it )
        std::cout << it->data << ' ';

    std::cout << nl;

    return EXIT_SUCCESS;
}

int main986986 ( ) {

    using namespace fst;

    auto x = rng.instance ( 123u );

    std::bernoulli_distribution b_dist1 ( 0.66 );
    std::bernoulli_distribution b_dist2 ( 0.33 );

    using Tree = SearchTree<MoveType, MovesType>;

    std::cout << sizeof ( Tree::Arc ) << nl;  // 32
    std::cout << sizeof ( Tree::Node ) << nl; // 512

    Tree t ( getMoves ( ) ); // Root Moves.

    std::uint64_t cnt = 1024 * 1024 * 4;

    typename Tree::NodeID node = t.root_node;

    plf::nanotimer timer;

    double elapsed = 0.0;

    {
        timer.start ( );

        while ( --cnt ) {

            while ( b_dist1 ( rng.instance ( ) ) and hasChild ( t, node ) ) {

                node = selectChild ( t, node );
            }

            if ( b_dist2 ( rng.instance ( ) ) and hasMoves ( t, node ) ) {

                addChild ( t, node );
            }

            node = t.root_node;
        }

        elapsed = timer.get_elapsed_ms ( );
    }

    std::cout << t.arcNum ( ) << " - " << t.nodeNum ( ) << nl << nl;

    std::cout << static_cast<std::uint64_t> ( elapsed ) << nl;

    return EXIT_SUCCESS;
}
