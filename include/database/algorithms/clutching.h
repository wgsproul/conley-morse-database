#ifndef CMDB_CLUTCHING_H
#define CMDB_CLUTCHING_H

#include <boost/foreach.hpp>
#include <algorithm>
#include <stack>
#include <vector>
#include <ctime>
#include <set>

#include "boost/serialization/vector.hpp"
#include "boost/serialization/map.hpp"
#include "boost/serialization/set.hpp"

#include "database/structures/MorseGraph.h"
#include "database/structures/Database.h"
#include "database/structures/Tree.h"

// Declaration
inline void Clutching( BG_Data * result,
               const MorseGraph & graph1,
               const MorseGraph & graph2 );

// Definition
inline void Clutching( BG_Data * result,
               const MorseGraph & graph1,
               const MorseGraph & graph2 ) {

  typedef MorseGraph::Vertex Vertex;
  std::set < std::pair < Vertex, Vertex > > bipartite_graph;

  size_t N1 = graph1 . NumVertices ();
  size_t N2 = graph2 . NumVertices ();
  
  // Dynamic dispatch.
  std::vector < std::vector < boost::shared_ptr<const TreeGrid> > > 
    graph1_trees, graph2_trees;
  size_t num_charts = 1;
  if ( boost::dynamic_pointer_cast<const Atlas> ( graph1 . phaseSpace () ) ) {
  	const Atlas & atlas1 = * boost::dynamic_pointer_cast<const Atlas> 
      ( graph1 . phaseSpace () );
  	const Atlas & atlas2 = * boost::dynamic_pointer_cast<const Atlas> 
      ( graph2 . phaseSpace () );

  	// Determine number of charts.
  	size_t num_charts1 = atlas1 . numCharts ();
  	size_t num_charts2 = atlas2 . numCharts ();
  	if ( num_charts1 != num_charts2 ) {
  		return; // No clutching due to being incompatible.
  	}
  	num_charts = num_charts1;
  	graph1_trees . resize ( num_charts, 
      std::vector < boost::shared_ptr<const TreeGrid> > ( N1 ) );
  	graph2_trees . resize ( num_charts, 
      std::vector < boost::shared_ptr<const TreeGrid> > ( N2 ) );

  	// Loop through vertices and charts
  	for ( size_t i = 0; i < N1; ++ i ) {
  		const Atlas & atlas = * boost::dynamic_pointer_cast<const Atlas> 
        ( graph1 . grid ( i ) );
      size_t count = 0;
  		for ( Atlas::ChartIteratorPair it_pair = atlas . charts ();
  				  it_pair . first != it_pair . second;
  				  ++ it_pair . first ) {
  			Atlas::Chart chart = it_pair . first -> second;
        if ( chart -> size () > 0 ) {
  			 graph1_trees[count][i] = chart;
        } else {
         graph1_trees[count][i] . reset ();
        }
        ++ count;
  		}
  	}
  	for ( size_t i = 0; i < N2; ++ i ) {
  		const Atlas & atlas = * boost::dynamic_pointer_cast<const Atlas> 
        ( graph2 . grid ( i ) );
      size_t count = 0;
  		for ( Atlas::ChartIteratorPair it_pair = atlas . charts ();
  				  it_pair . first != it_pair . second;
  				  ++ it_pair . first ) {
  			Atlas::Chart chart = it_pair . first -> second;
        if ( chart -> size () > 0 ) {
         graph2_trees[count][i] = chart;
        } else {
         graph2_trees[count][i] . reset ();
        }
        ++ count;
  		}
  	}
  }

  if ( boost::dynamic_pointer_cast<const TreeGrid> ( graph1 . phaseSpace () ) ) {
  	graph1_trees . resize ( num_charts, 
      std::vector < boost::shared_ptr<const TreeGrid> > ( N1 ) );
  	graph2_trees . resize ( num_charts, 
      std::vector < boost::shared_ptr<const TreeGrid> > ( N2 ) );
// Loop through vertices and charts
  	for ( size_t i = 0; i < N1; ++ i ) {
  		graph1_trees[0][i] = boost::dynamic_pointer_cast<const TreeGrid> 
        ( graph1 . grid ( i ) );
  	}
  	for ( size_t i = 0; i < N2; ++ i ) {
  		graph2_trees[0][i] = boost::dynamic_pointer_cast<const TreeGrid> 
        ( graph2 . grid ( i ) );
  	}
  }

  // Loop through charts.
  // For each chart, make a collection of tree references
  // Modify the algorithm to use the references rather than -> grid ( i) . tree ()
  for ( size_t chart_id = 0; chart_id < num_charts; ++ chart_id ) {
  	const std::vector< boost::shared_ptr<const TreeGrid> > & trees1 
      = graph1_trees [ chart_id ];
  	const std::vector< boost::shared_ptr<const TreeGrid> > & trees2 
      = graph2_trees [ chart_id ];

  	typedef Tree::iterator iterator;

  // How this works:
  // We want to advance through the trees simultaneously, but they 
  // aren't all the same tree. If we explore a subtree in some trees 
  // that does not exist in others, we remain halted on the others
  // until the subtree finishes.
  //
  // initialize iterators

  // State machine:
  // State 0: Try to go left. If can't, set success to false and try to go right. Otherwise success is true and try to go left on next iteration.
  // State 1: Try to go right. If can't, set success to false and rise. Otherwise success is true and try to go left on next iteration.
  // State 2: Rise. If rising from the right, rise again on next iteration. Otherwise try to go right on the next iteration.
  	std::vector < iterator > iters1 ( N1 );
  	std::vector < iterator > iters2 ( N2 );
  	for ( size_t i = 0; i < N1; ++ i ) 
      if ( trees1[i] ) iters1[i] = trees1[i] -> treeBegin ();
  	for ( size_t i = 0; i < N2; ++ i ) 
      if ( trees2[i] ) iters2[i] = trees2[i] -> treeBegin ();
  	std::vector < size_t > depth1 ( N1, 0 );
  	std::vector < size_t > depth2 ( N2, 0 );
  	size_t depth = 0;
  	int state = 0;

  //std::cout << "Clutching Function.\n";
  //std::cout << "N1 = " << N1 << " and N2 = " << N2 << "\n";
  		while ( 1 ) {
  			if ( (depth == 0) && ( state == 2 ) ) break;
    //std::cout << "Position 0. depth = " << depth << " and state = " << state << "\n";
  			bool success = false;
  			Vertex set1 = N1;
  			Vertex set2 = N2;
  			for ( size_t i = 0; i < N1; ++ i ) {
          if ( not trees1 [ i ] ) continue;
      //std::cout << "Position 1. i = " << i << ", depth = " << depth << " and state = " << state << "\n";
      // If node is halted, continue
  				if ( depth1[i] == depth ) {
  					iterator end = trees1[i] -> treeEnd ();
  					switch ( state ) {
  						case 0:
  						{
  							iterator left = trees1[i] -> left ( iters1 [ i ] );
  							if ( left == end ) break;
  							iters1[i] = left;
  							++ depth1[i];
  							success = true;
  							break;
  						}
  						case 1:
  						{
  							iterator right = trees1 [ i ] -> right ( iters1 [ i ] );
  							if ( right == end ) break;
  							iters1[i] = right;
  							++ depth1[i];
  							success = true;
  							break;
  						}
  						case 2:
  						{
  							if ( trees1 [ i ] -> tree () . isRight ( iters1 [ i ] ) ) 
                    success = true;
  							iters1[i] = trees1 [ i ] -> parent ( iters1 [ i ] );
  							-- depth1[i];
  							break;
  						}
  					}
  				}
  				if ( trees1 [ i ] -> isGrid ( iters1 [ i ] ) ) {
  					if ( set1 != (Vertex)N1 ) std::cout << "Warning, morse sets are not disjoint.\n";
  					set1 = (Vertex)i;
  				}

  			}
  			for ( size_t i = 0; i < N2; ++ i ) {
          if ( not trees2 [ i ] ) continue;
      //std::cout << "Position 2. i = " << i << ", depth = " << depth << " and state = " << state << "\n";
      // If node is halted, continue
  				if ( depth2[i] == depth ) {
  					iterator end = trees2 [ i ] -> treeEnd ();
  					switch ( state ) {
  						case 0:
  						{
  							iterator left = trees2 [ i ] -> left ( iters2 [ i ] );
  							if ( left == end ) break;
  							iters2[i] = left;
  							++ depth2[i];
  							success = true;
  							break;
  						}
  						case 1:
  						{
  							iterator right = trees2 [ i ] -> right ( iters2 [ i ] );
  							if ( right == end ) break;
  							iters2[i] = right;
  							++ depth2[i];
  							success = true;
  							break;
  						}
  						case 2:
  						{
  							if ( trees2 [ i ] -> tree () . isRight ( iters2 [ i ] ) ) 
                    success = true;
  							iters2[i] = trees2 [ i ] -> parent ( iters2 [ i ] );
  							-- depth2[i];
  							break;
  						}
  					}
  				}
  				if ( trees2 [ i ] -> isGrid ( iters2 [ i ] ) ) {
  					if ( set2 != (Vertex)N2 ) std::cout << "Warning, morse sets are not disjoint.\n";
  					set2 = (Vertex)i;
  				}
  			}
    //std::cout << "Position 3. depth = " << depth << " and state = " << state << "\n";
    //std::cout << ( success ? "success" : "failure" );
  			switch ( state ) {
      case 0: // Tried to go left
      if ( success ) {
          // Success. Try to go left again.
      	state = 0;
      	++ depth;
      } else {
          // Failure. Try to go right instead.
      	state = 1;
      }
      break;
      case 1: // Tried to go right
      if ( success ) {
          // Sucess. Try to go left now.
      	state = 0;
      	++ depth;
      } else {
          // Failure. Rise.
      	state = 2;
      }
      break;
      case 2: // Rose
      -- depth;
      if ( success ) {
          // Rose from right, continue to rise
      	state = 2;
      } else {
          // Rose from left, try to go right
      	state = 1;
      }
      break;
    }
    // Gather all intersection information
    if ( (set1 != (Vertex)N1 ) && (set2 != (Vertex)N2 ) ) {
      //std::cout << "Record intersection (" << set1 << ", " << set2 << ")\n";

    	bipartite_graph . insert ( std::pair < Vertex, Vertex > ( set1, set2 ) );
    }
  }
}
  // Advance iterators to end, collecting intersections
  // Return result
  //std::cout << "Collate Results.\n";
  BOOST_FOREACH ( const MorseGraph::Edge & edge, bipartite_graph ) {
    result -> edges . push_back ( edge );
  }
}

#endif
