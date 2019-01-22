
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
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

#include "splitmix.hpp"
#include "singleton.hpp"
#include "flat_search_tree.hpp"
#include "adjacency_search_tree.hpp"
#include "link.hpp"
#include "path.hpp"
#include "mcts_emu.hpp"
#include "moves.hpp"


#define nl L'\n'


Singleton<splitmix64> rng;


int wmain ( ) {

    using namespace fst;

    rng.instance ( 123u );

    using Tree = SearchTree<int, int>;
    using Node = typename Tree::NodeID;
    using Arc = typename Tree::ArcID;

    std::wcout << sizeof ( Tree::Arc ) << nl;  //  32
    std::wcout << sizeof ( Tree::Node ) << nl; // 512

    Tree t ( 1 );

    Node n2 = t.addNode ( 2 );
    Arc a1 = t.addArc ( t.root_node, n2, 1 );

    Node n3 = t.addNode ( 3 );
    Arc a2 = t.addArc ( t.root_node, n3, 2 );

    Node n4 = t.addNode ( 4 );
    Arc a3 = t.addArc ( t.root_node, n4, 3 );

    Node n5 = t.addNode ( 5 );
    Arc a4 = t.addArc ( n2, n5, 4 );
    Arc a5 = t.addArc ( n3, n5, 5 );

    Node n6 = t.addNode ( 6 );
    Arc a6 = t.addArc ( n3, n6, 6 );

    Node n7 = t.addNode ( 7 );
    Arc a7 = t.addArc ( n3, n7, 7 );

    Node n8 = t.addNode ( 8 );
    Arc a8 = t.addArc ( n4, n8, 8 );

    Node n9 = t.addNode ( 9 );
    Arc a9 = t.addArc ( n5, n9, 9 );
    Arc a10 = t.addArc ( n6, n9, 10 );

    Node n10 = t.addNode ( 10 );
    Arc a11 = t.addArc ( n6, n10, 11 );
    Arc a12 = t.addArc ( n7, n10, 12 );
    Arc a13 = t.addArc ( n8, n10, 13 );

    Node n11 = t.addNode ( 11 );
    Arc a14 = t.addArc ( n8, n11, 14 );

    Arc a15 = t.addArc ( n2, n8, 15 );

    std::wcout << t.arcNum ( ) << L" - " << t.nodeNum ( ) << nl << nl;

    Tree s = t.makeSubTree ( Node { 2 } );

    std::wcout << s.arcNum ( ) << L" - " << s.nodeNum ( ) << nl << nl;

    t.traverseBreadthFirst ( );

    std::wcout << nl << nl;

    t.traverseDepthFirst  ( );

    std::wcout << nl << nl;

    const auto sorted { t.topologicalSort ( ) };

    for ( auto v : sorted ) {
        std::wcout << v << L' ';
    }
    std::wcout << L'\n';

    std::wcout << nl << nl;

    return EXIT_SUCCESS;
}



int wmain67689 ( ) {

    using namespace fst;

    rng.instance ( 123u );

    std::bernoulli_distribution b_dist1 ( 0.66 );
    std::bernoulli_distribution b_dist2 ( 0.33 );

    using Tree = SearchTree<MoveType, MovesType>;

    std::wcout << sizeof ( Tree::Arc ) << nl; //32
    std::wcout << sizeof ( Tree::Node ) << nl; //512

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

    std::wcout << t.arcNum ( ) << L" - " << t.nodeNum ( ) << nl << nl;

    std::wcout << static_cast<std::uint64_t> ( elapsed ) << nl;

    return EXIT_SUCCESS;
}