
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

#include "splitmix.hpp"
#include "singleton.hpp"
#include "tree.hpp"
#include "mcts_emu.hpp"
#include "moves.hpp"
#include "link.hpp"
#include "path.hpp"

#define nl L'\n'


Singleton<splitmix64> rng;


int wmain8989789 ( ) {

    rng.instance ( 123u );

    std::wcout << getMoves ( );
    std::wcout << getMoves ( );
    std::wcout << getMoves ( );
    std::wcout << getMoves ( );

    return EXIT_SUCCESS;
}


int wmain ( ) {

    rng.instance ( 123u );

    using Tree = SearchTree<MoveType, MovesType>;

    Tree g ( getMoves ( ) ); // Root Moves...

    // Layer 1...

    NodeID node1 = addNode ( g, g.root_node, true );
    NodeID node2 = addNode ( g, g.root_node, true );
    NodeID node3 = addNode ( g, g.root_node, true );

    // Layer 2...

    NodeID node4 = addNode ( g, node1, true );
    addArc ( g, node2, node4, true );
    addArc ( g, node3, node4, true );

    NodeID node5 = addNode ( g, node1, true );
    addArc ( g, node2, node5, true );
    addArc ( g, node3, node5, true );

    // Layer 3...

    NodeID node6 = addNode ( g, node4, true );
    addArc ( g, node5, node6, true );

    NodeID node7 = addNode ( g, node4, true );
    addArc ( g, node5, node7, true );

    NodeID node8 = addNode ( g, node4, true );
    addArc ( g, node5, node8, true );

    std::wcout << nl;

    std::wcout << g.arcNum ( ) << L" " << g.nodeNum ( ) << nl;

    return EXIT_SUCCESS;
}
