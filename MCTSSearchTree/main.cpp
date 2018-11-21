
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <boost/container/deque.hpp> // MSVC STL-deque is no good.
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

#define NOGDI

#include <plf/plf_nanotimer.h>

#include "splitmix.hpp"
#include "singleton.hpp"
#include "tree.hpp"
#include "mcts_emu.hpp"
#include "moves.hpp"
#include "link.hpp"
#include "path.hpp"

#define nl L'\n'


Singleton<splitmix64> rng;


int wmaineewrw ( ) {

    rng.instance ( 123u );

    using Tree = SearchTree<MoveType, MovesType>;

    std::wcout << sizeof ( Tree::Arc ) << nl; //32
    std::wcout << sizeof ( Tree::Node ) << nl; //512

    Tree t ( getMoves ( ) ); // Root Moves...

    // Layer 1...

    NodeID node1 = addNode ( t, t.root_node );
    NodeID node2 = addNode ( t, t.root_node );
    NodeID node3 = addNode ( t, t.root_node );

    for ( auto it = t.beginOut ( t.root_node ); it.is_valid ( ); ++it ) {
        std::wcout << *it << nl;
    }

    // Layer 2...

    NodeID node4 = addNode ( t, node1 );
    addArc ( t, node2, node4 );
    addArc ( t, node3, node4 );

    NodeID node5 = addNode ( t, node1 );
    addArc ( t, node2, node5 );
    addArc ( t, node3, node5 );

    // Layer 3...

    NodeID node6 = addNode ( t, node4 );
    addArc ( t, node5, node6 );

    NodeID node7 = addNode ( t, node4 );
    addArc ( t, node5, node7 );

    NodeID node8 = addNode ( t, node4 );
    addArc ( t, node5, node8 );

    std::wcout << nl;

    std::wcout << t.arcNum ( ) << L" " << t.nodeNum ( ) << nl;

    return EXIT_SUCCESS;
}


int wmain ( ) {

    rng.instance ( 123u );

    std::bernoulli_distribution b_dist1 ( 0.66 );
    std::bernoulli_distribution b_dist2 ( 0.33 );

    using Tree = SearchTree<MoveType, MovesType>;

    std::wcout << sizeof ( Tree::Arc ) << nl; //32
    std::wcout << sizeof ( Tree::Node ) << nl; //512

    Tree t ( getMoves ( ) ); // Root Moves...

    std::uint64_t cnt = 1024 * 1024 * 4;

    NodeID node = t.root_node;

    plf::nanotimer timer;

    double elapsed = 0.0;

    {
        timer.start ( );

        while ( --cnt ) {

            while ( b_dist1 ( rng.instance ( ) ) and hasChild ( t, node ) ) {

                node = selectChild ( t, node );
            }

            if ( b_dist2 ( rng.instance ( ) ) and hasMoves ( t, node ) ) {

                addNode ( t, node );
            }

            node = t.root_node;
        }

        elapsed = timer.get_elapsed_ms ( );
    }

    std::wcout << t.arcNum ( ) << L" - " << t.nodeNum ( ) << nl << nl;

    std::wcout << static_cast<std::uint64_t> ( elapsed ) << nl;

    return EXIT_SUCCESS;
}