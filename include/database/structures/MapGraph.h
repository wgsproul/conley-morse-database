// MapGraph.h

#ifndef CMDP_MAPGRAPH_H
#define CMDP_MAPGRAPH_H

#include <vector>
#include <iterator>
#include <iostream>
#include "boost/unordered_map.hpp"
#include "boost/foreach.hpp"

template < class Toplex, class Map, class CellContainer >
class MapGraph {
public:
  typedef typename Toplex::size_type size_type;
  typedef typename Toplex::Top_Cell Vertex;
  
  MapGraph ( const Toplex & t, 
             const Map & f );
  
  MapGraph ( const std::vector < CellContainer > & sets, 
             const Toplex & t,
             const Map & f);
 
  size_type insert ( const Vertex & v );

  std::vector<size_type> adjacencies ( const size_type & v ) const;
  size_type sentinel ( void ) const;
  size_type index ( const Vertex & input ) const;
  size_type num_vertices ( void ) const;
  Vertex lookup ( const size_type & input ) const;
  void index ( std::vector < size_type > * output, const CellContainer & input ) const;
  void lookup ( CellContainer * output, const std::vector < size_type > & input ) const;
  void leaves ( CellContainer * output, const std::vector < size_type > & input ) const;

private:
  const Toplex & toplex_;
  const Map & f_;
  std::vector<Vertex> lookup_;
  std::vector<size_type> index_;
  //boost::unordered_map<Vertex,size_type> index_;
  size_type sentinel_;
};

template < class Toplex, class Map, class CellContainer >
MapGraph<Toplex,Map,CellContainer>::
MapGraph ( const Toplex & t, 
           const Map & f ) : 
toplex_ ( t ),
f_ ( f ),
sentinel_ ( t . tree_size () ) {
  index_ . resize ( sentinel (), sentinel () ); 
  CellContainer allcells;
  std::insert_iterator < CellContainer > ii ( allcells, 
                                             allcells . begin () );
  toplex_ . cover ( ii, toplex_ . bounds () );
  CellContainer umbrella;
  std::insert_iterator < CellContainer > uii ( umbrella, 
                                              umbrella . begin () );
  toplex_ . umbrella ( uii, allcells );
  BOOST_FOREACH ( Vertex v, umbrella ) {
    insert ( v );
  }
}

template < class Toplex, class Map, class CellContainer >
MapGraph<Toplex,Map,CellContainer>::
MapGraph ( const std::vector < CellContainer > & sets, 
           const Toplex & t, 
           const Map & f ) : 
toplex_ ( t ),
f_ ( f ),
sentinel_ ( t . tree_size () ) {
  //std::cout << "Constructing MapGraph.\n";
  index_ . resize ( sentinel (), sentinel () ); 

  CellContainer allcells;
  std::insert_iterator < CellContainer > ii ( allcells, 
                                              allcells . begin () );
  BOOST_FOREACH ( const CellContainer & cont, sets ) {
    BOOST_FOREACH ( Vertex v, cont ) {
      //std::cout << "Inserting " << v << " into allcells.\n";
      * ii ++ = v;
    }
  }
  CellContainer umbrella;
  std::insert_iterator < CellContainer > uii ( umbrella, 
                                               umbrella . begin () );
  toplex_ . umbrella ( uii, allcells );
  BOOST_FOREACH ( Vertex v, umbrella ) {
    //std::cout << "Inserting " << v << " into graph.\n";
    insert ( v );
  }
  //std::cout << "Finished constructing MapGraph.\n";

}

template < class Toplex, class Map, class CellContainer >
typename MapGraph<Toplex,Map,CellContainer>::size_type 
MapGraph<Toplex,Map,CellContainer>::
insert ( const Vertex & v ) {
  index_ [ v ] = lookup_ . size ();
  lookup_ . push_back ( v );
  return index_ [ v ];
}

template < class Toplex, class Map, class CellContainer >
std::vector<typename MapGraph<Toplex,Map,CellContainer>::size_type>
MapGraph<Toplex,Map,CellContainer>::
adjacencies ( const size_type & source ) const {
  //std::cout << "Computing adjacencies of vertex " << source << "\n";
  std::vector < size_type > result;
  Vertex domain_cell = lookup ( source );
  //std::cout << " This is grid element " << domain_cell << "\n";
  CellContainer children;
  std::insert_iterator < CellContainer > cii ( children, children . begin () );
  toplex_ . children ( cii, domain_cell );
  if ( children . empty () ) {
    //std::cout << "It is a leaf, so we do a computation.\n";
    CellContainer image;
    std::insert_iterator < CellContainer > ii ( image, image . begin () );
    toplex_ . coarseCover ( ii, f_ ( toplex_ . geometry ( domain_cell ) ) );

    /*
    CellContainer imageleaves;
    CellContainer debugimage;
    CellContainer debugleaves;
    std::insert_iterator < CellContainer > lii ( imageleaves, imageleaves . begin () );
    std::insert_iterator < CellContainer > dii ( debugimage, debugimage . begin () );
    std::insert_iterator < CellContainer > ldii ( debugleaves, debugimage . begin () );

    toplex_ . cover ( dii, f_ ( toplex_ . geometry ( domain_cell ) ) );
    toplex_ . leaves ( lii, image );
    toplex_ . leaves ( ldii, debugimage );
    
    std::cout << " ------------------n";
    std::cout << "Usual image:\n";
    BOOST_FOREACH ( GridElement g, debugimage ) {
      std::cout << g << " ";
    }
    std::cout << "\n";
    
    std::cout << "Coarse image:\n";
    BOOST_FOREACH ( GridElement g, image ) {
      std::cout << g << " ";
    }
    std::cout << "\n";
    
    std::cout << "Leaves of coarse image:\n";
    BOOST_FOREACH ( GridElement g, imageleaves ) {
      std::cout << g << " ";
    }
    std::cout << "\n";

    std::cout << "Leaves of usual image:\n";
    BOOST_FOREACH ( GridElement g, debugleaves ) {
      std::cout << g << " ";
    }
    std::cout << "\n";
    */
    index ( & result, image );
  } else {
    //std::cout << "It is not a leaf, so we give the children.\n";
    index ( & result, children ); 
  }
  //char c;
  //std::cin >> c;
  //std::cout << "Adjacency computation complete.\n";
  return result; 
}


template < class Toplex, class Map, class CellContainer >
typename MapGraph<Toplex,Map,CellContainer>::size_type 
MapGraph<Toplex,Map,CellContainer>::
sentinel ( void ) const {
  return sentinel_;
}

template < class Toplex, class Map, class CellContainer >
typename MapGraph<Toplex,Map,CellContainer>::size_type 
MapGraph<Toplex,Map,CellContainer>::
index ( const Vertex & input ) const {
  return index_ [ input ];
  //typename boost::unordered_map<Vertex,size_type>::const_iterator it;
  //it = index_ . find ( input );
  //if ( it == index_ . end () ) return sentinel ();
  //return it -> second;
}

template < class Toplex, class Map, class CellContainer >
typename MapGraph<Toplex,Map,CellContainer>::Vertex 
MapGraph<Toplex,Map,CellContainer>::
lookup ( const size_type & input ) const {
  return lookup_ [ input ];
}

template < class Toplex, class Map, class CellContainer >
void MapGraph<Toplex,Map,CellContainer>::
index ( std::vector < size_type > * output, 
        const CellContainer & input ) const {
  output -> clear ();
  BOOST_FOREACH ( Vertex cell, input ) {
    size_type cell_index = index ( cell );
    if ( cell_index != sentinel () ) output -> push_back ( cell_index );
  }
}

template < class Toplex, class Map, class CellContainer >
void MapGraph<Toplex,Map,CellContainer>::
lookup ( CellContainer * output, 
         const std::vector < size_type > & input ) const {
  output -> clear ();
  std::insert_iterator < CellContainer > ii ( *output, output -> begin () );
  BOOST_FOREACH ( size_type cell_index, input ) {
    * ii ++ = lookup ( cell_index );
  }
}

template < class Toplex, class Map, class CellContainer >
void MapGraph<Toplex,Map,CellContainer>::
leaves ( CellContainer * output, 
        const std::vector < size_type > & input ) const {
  output -> clear ();
  std::insert_iterator < CellContainer > ii ( *output, output -> begin () );
  BOOST_FOREACH ( size_type cell_index, input ) {
    Vertex domain_cell = lookup ( cell_index );
    CellContainer children;
    std::insert_iterator < CellContainer > cii ( children, children . begin () );
    toplex_ . children ( cii, domain_cell );
    if ( children . empty () ) * ii ++ = domain_cell;
  }
}

template < class Toplex, class Map, class CellContainer >
typename MapGraph<Toplex,Map,CellContainer>::size_type 
MapGraph<Toplex,Map,CellContainer>::num_vertices ( void ) const {
  return lookup_ . size ();
}

#endif