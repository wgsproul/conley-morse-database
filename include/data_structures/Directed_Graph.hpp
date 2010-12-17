/*
 *  Directed_Graph.hpp
 */

#include <functional>

/* BOOST includes */
//#include <boost/config.hpp>
//#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp> 
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/properties.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/topological_sort.hpp>

/* External Adaptation of DirectedGraph so it is a BGL graph as well */

namespace DirectedGraph_detail {
  
template < class Toplex >
class DirectedGraph_out_edge_iterator : public Toplex::Subset::const_iterator {
 public:
  /* Typedefs */
  typedef typename Toplex::Top_Cell vertex_descriptor;
  typedef std::pair<vertex_descriptor, vertex_descriptor> edge_descriptor;
  /* Default Constructor */
  DirectedGraph_out_edge_iterator ( void ) {}
  /* Constructor */
  DirectedGraph_out_edge_iterator (const vertex_descriptor & v, 
                                   const typename Toplex::Subset::const_iterator & it ) :
      Toplex::Subset::const_iterator ( it ), domain_cell_ ( v ) {}
  /* Dereference */
  const edge_descriptor operator * ( void ) {
    return std::pair < vertex_descriptor, vertex_descriptor> 
        ( domain_cell_, Toplex::Subset::const_iterator::operator * () );
  }
  /* Preincrement */
  DirectedGraph_out_edge_iterator & operator ++ ( void ) {
    Toplex::Subset::const_iterator::operator ++ ();
    return *this;
  } /* operator ++ */
  /* Postincrement */
  DirectedGraph_out_edge_iterator operator ++ ( int ) {
    return DirectedGraph_out_edge_iterator ( domain_cell_, 
                                             Toplex::Subset::const_iterator::operator ++ ( int () ) );
  } /* operator ++ */
 private:
  typename Toplex::Top_Cell domain_cell_;
};

template < class Toplex >
class DirectedGraph_vertex_iterator : public DirectedGraph<Toplex>::const_iterator {
 public:
  /* Typedefs */
  typedef typename Toplex::Top_Cell vertex_descriptor;
  /* Default Constructor */
  DirectedGraph_vertex_iterator ( void ) {}
  /* Constructor */
  DirectedGraph_vertex_iterator ( const typename DirectedGraph<Toplex>::const_iterator & it ) :
      DirectedGraph<Toplex>::const_iterator ( it ) {}
  /* Dereference */
  const vertex_descriptor & operator * ( void ) {
    return DirectedGraph<Toplex>::const_iterator::operator * () . first;
  } /* operator * */
  /* Preincrement */
  DirectedGraph_vertex_iterator & operator ++ ( void ) {
    DirectedGraph<Toplex>::const_iterator::operator ++ ();
    return *this;
  } /* operator ++ */
  /* Postincrement */
  DirectedGraph_vertex_iterator operator ++ ( int ) {
    return DirectedGraph_vertex_iterator 
        ( DirectedGraph<Toplex>::const_iterator::operator ++ ( int () ) );
  } /* operator ++ */
  const vertex_descriptor * operator -> ( void ) {
    return & DirectedGraph<Toplex>::const_iterator::operator * () . first;
  } /* operator -> */
};

struct graph_traversal_category : 
      public virtual boost::incidence_graph_tag,
      public virtual boost::adjacency_graph_tag,
      public virtual boost::vertex_list_graph_tag { };
  
} /* namespace DirectedGraph_detail */

namespace boost {
template < class Toplex >
struct graph_traits< DirectedGraph<Toplex> > {    
  /* Vertex and Edge descriptor */
  typedef typename Toplex::Top_Cell vertex_descriptor;
  typedef std::pair<vertex_descriptor, vertex_descriptor> edge_descriptor;
  /* Graph categories */
  typedef boost::directed_tag directed_category;
  typedef boost::disallow_parallel_edge_tag edge_parallel_category;
  typedef DirectedGraph_detail::graph_traversal_category traversal_category;
  /* Size Types */
  typedef typename Toplex::size_type degree_size_type;
  typedef typename Toplex::size_type vertices_size_type;
  /* Iterators */
  typedef typename Toplex::Subset::const_iterator adjacency_iterator;
  typedef DirectedGraph_detail::DirectedGraph_out_edge_iterator<Toplex> out_edge_iterator;
  typedef DirectedGraph_detail::DirectedGraph_vertex_iterator<Toplex> vertex_iterator;
  /* Member functions */
  vertex_descriptor null_vertex ( void ) {
    return ( typename Toplex::Top_Cell ) ( -1 ); 
  } /* boost::graph_traits<>::null_vertex */
};
template < class Toplex >
std::pair< typename graph_traits< DirectedGraph<Toplex> >::adjacency_iterator, 
           typename graph_traits< DirectedGraph<Toplex> >::adjacency_iterator > 
adjacent_vertices ( typename graph_traits< DirectedGraph<Toplex> >::vertex_descriptor v, 
                    const DirectedGraph<Toplex> & g ) {
  return std::pair< typename graph_traits< DirectedGraph<Toplex> >::adjacency_iterator, 
      typename graph_traits< DirectedGraph<Toplex> >::adjacency_iterator > 
      ( g . find ( v ) -> second . begin (), g . find ( v ) -> second . end () );
} /* boost::adjacent_vertices<> */
  
/* For some reason the BGL has already implemented the following and we get ambiguity.
   The problem is that these are overloads and not specialization. 
   Luckily, the defaults are precisely what we want. 
*/
/*
  template < class Toplex >
  typename graph_traits< DirectedGraph<Toplex> >::vertex_descriptor 
  source(const typename graph_traits< DirectedGraph<Toplex> >::edge_descriptor & e, 
  const DirectedGraph<Toplex> & g) {	
  return e . first;
  } 
*//* source */
  
/*
  template < class Toplex >
  typename graph_traits< DirectedGraph<Toplex> >::vertex_descriptor
  target(const typename graph_traits< DirectedGraph<Toplex> >::edge_descriptor & e, 
  const DirectedGraph<Toplex> & g) {
  return e . second;
  } 
*//* target */
  
template < class Toplex >
std::pair<typename graph_traits< DirectedGraph<Toplex> >::out_edge_iterator, 
          typename graph_traits< DirectedGraph<Toplex> >::out_edge_iterator > 
out_edges(const typename graph_traits< DirectedGraph<Toplex> >::vertex_descriptor & u, 
          const DirectedGraph<Toplex> & g) {
  const typename Toplex::Subset & subset = g . find ( u ) -> second;
  /*
  //DEBUG
  if ( g . find ( u ) == g . end () ) {
    std::cout << "Fatal out_edges flaw: vertex " << u << " isn't in graph.\n";
    //exit ( -1 );
  } 
  //END DEBUG
  */
  typedef typename graph_traits< DirectedGraph<Toplex> >::out_edge_iterator edge_iter;
  return std::make_pair (edge_iter ( u, subset . begin () ),
                         edge_iter ( u, subset . end () ) );
} /* out_edges */
  
template < class Toplex >
typename graph_traits< DirectedGraph<Toplex> >::degree_size_type  
out_degree(const typename graph_traits< DirectedGraph<Toplex> >::vertex_descriptor & u, 
           const DirectedGraph<Toplex> & g) {
  return g . find ( u ) -> second . size ();
} /* out_degree */
  
template < class Toplex >
std::pair<typename graph_traits< DirectedGraph<Toplex> >::vertex_iterator, 
          typename graph_traits< DirectedGraph<Toplex> >::vertex_iterator>
vertices(const DirectedGraph<Toplex> & g) {
  typedef typename graph_traits< DirectedGraph<Toplex> >::vertex_iterator Iterator;
  return std::make_pair ( Iterator (g.begin()), Iterator (g.end()) );    
} /* vertices */
  
template < class Toplex >
typename graph_traits< DirectedGraph<Toplex> >::vertices_size_type 
num_vertices(const DirectedGraph<Toplex> & g)	{
  return g . size ();
} /* num_vertices */
  
} /* namespace boost */

template < class Toplex >
void DirectedGraph<Toplex>::removeVertex (typename Toplex::Top_Cell v)
{
  (*this).erase(v);
  typename DirectedGraph<Toplex>::iterator graph_it;

  graph_it = (*this).begin();
  while (graph_it != (*this).end()) {
    ((*graph_it).second).erase(v);
    ++graph_it;
  }
}

template < class Toplex >
DirectedGraph<Toplex> collapseComponents (
    const DirectedGraph<Toplex> & G,
    const typename DirectedGraph<Toplex>::Components & Components,
    std::vector<typename DirectedGraph<Toplex>::Vertex> & Representatives)
{
  typedef DirectedGraph <Toplex> Graph;
  typedef typename DirectedGraph<Toplex>::Vertex Vertex;
  typedef typename boost::graph_traits<DirectedGraph<Toplex> >::vertex_iterator vi;
  typedef typename boost::graph_traits<DirectedGraph<Toplex> >::adjacency_iterator ai; // Toplex_Subset iterators

  std::map < Vertex, Vertex > table;

  Graph result(G);
  Vertex compRep; // The representative of a component

  // Iterators
  typename Graph::iterator graph_it;
  typename Graph::Components::const_iterator comp_it;
  typename Graph::Component::const_iterator vert_it;
  typename Toplex::Subset::const_iterator edge_it;
  
  // For each component, remove the vertices of the component
  // except the first one, which will be the representative.
  // The representative collects edges of removed vertices.
  // The 'table' variable, which will be used to replace the
  // destinations of edges, is constructed at the same time.
  Representatives.clear();
  comp_it = Components.begin();
  // Iterate through components
  while (comp_it != Components.end()) {
    vert_it = (*comp_it).begin();
    compRep = (*vert_it);
    //std::cout << "Processing compRep " << compRep << "\n";
    ++vert_it;
    // Iterate through vertices in component (except for the first, dubbed the representative)
    while (vert_it != (*comp_it).end()) {
      //std::cout << "  contains vertex " << * vert_it << "\n";
      // Give all edges to the representative
      edge_it = G . find ( *(vert_it) ) -> second .begin();
      while (edge_it != G . find ( *(vert_it) ) -> second . end()) {
        result[compRep].insert(*edge_it);
        ++edge_it;
      }
      //result[compRep].insert(G[*(vert_it)].begin(), G[*(vert_it)].end());
      result.erase(*vert_it);
      table.insert(typename std::map< Vertex, Vertex >::value_type((*vert_it), compRep));
      ++vert_it;
    }
    ++comp_it;
    Representatives.push_back(compRep);
  }

  // Iterate through the vertices of the graph
  vi source_it, end_of_graph;
  for (boost::tie ( source_it, end_of_graph ) = boost::vertices ( result );
       source_it != end_of_graph; ++ source_it ) {
    typename Toplex::Subset target_set;
    Vertex source_vertex = * source_it;
    // Iterate through the target vertices of the current vertex
    ai target_it, end_of_target;
    for (boost::tie ( target_it, end_of_target ) = boost::adjacent_vertices ( source_vertex, result );
         target_it != end_of_target; ++ target_it ) {
      Vertex target_vertex = * target_it;
      // If the target vertex has been eliminated, then insert the representative instead.
      typename std::map <Vertex, Vertex>::const_iterator new_target_it = table . find ( target_vertex );
      if ( new_target_it != table . end () ) {
        // Target the representative instead
        target_vertex = new_target_it -> second;
      } /* if */
      // Unless its a self-loop, add it.
      if ( target_vertex != source_vertex ) {
        //std::cout << "Inserting edge (" << source_vertex << ", " << target_vertex << ")\n";
        target_set . insert ( target_vertex );
      } /* if-else */
    } /* for */
    result [ source_vertex ] = target_set;
  } /* for */

  //debug
  /*
  std::cout << "collapse components ----\n";
  typedef const std::pair < const typename Toplex::Top_Cell, typename Toplex::Subset > DG_value;
  
  BOOST_FOREACH ( DG_value & x, G ) {
    BOOST_FOREACH ( typename Toplex::Top_Cell target_cell, x . second ) {
      if ( G . find ( target_cell ) == G . end () ) std::cout << " G [ " << x . first << "] has " <<
        target_cell << " yet " << target_cell << " is not in the domain of G\n";
    }
  }
  
  BOOST_FOREACH ( DG_value & x, result ) {
    BOOST_FOREACH ( typename Toplex::Top_Cell target_cell, x . second ) {
      if ( result . find ( target_cell ) == result . end () ) std::cout << " result [ " << x . first << "] has " <<
        target_cell << " yet " << target_cell << " is not in the domain of result\n";
      if ( target_cell == x . first ) std::cout << " result [ " << x . first << "] has " <<
        target_cell << " yet " << target_cell << " we said self-loops were eliminated \n";
    }
  }
   */
  /* what if... the only thing mapping to some vertex in a morse set
     are the other vertices in the morse set... huh */
  // Return 
  return result;
}

template < class Toplex >
typename Toplex::Subset & DirectedGraph<Toplex>::operator () ( const typename Toplex::Top_Cell & cell ) {
  return operator [] ( cell );
} /* DirectedGraph<Toplex>::operator () for Top Cells */

template < class Toplex >
const typename Toplex::Subset & DirectedGraph<Toplex>::operator () ( const typename Toplex::Top_Cell & cell ) const {
  return find ( cell ) -> second;
} /* DirectedGraph<Toplex>::operator () for Top Cells */

// typename Toplex::Subset DirectedGraph<Toplex>::operator () ( const typename Toplex::Subset & subset ) {
//   typename Toplex::Subset return_value;
//   /* Loop through subset */
//   for ( typename Toplex::Subset::const_iterator it = subset . begin ();
//         it != subset . end (); ++ it ) {
//     const typename Toplex::Subset & value = operator [] ( * it );
//     for ( typename Toplex::Subset::const_iterator range_it = value . begin ();
//          range_it != value . end (); ++ range_it ) {
//       return_value . insert ( * range_it );
//     } /* for */
//   } /* for */
//   return return_value;
// } /* DirectedGraph<Toplex>::operator () for Subsets */

/* Global functions */

/* A wrapper for the boost's strongly connected components algorithm */
#include <boost/graph/strong_components.hpp>
template < class Toplex >
void computeSCC 
(std::vector < typename Toplex::Subset > * morse_sets,
 std::vector < typename Toplex::Subset > * entrance_sets,
 std::vector < typename Toplex::Subset > * exit_sets,
 const typename Toplex::Subset & entrance,
 const typename Toplex::Subset & exit,
 const DirectedGraph<Toplex> & G) {
  using namespace boost;

  typedef DirectedGraph<Toplex> Graph;
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  
  /* Pair Associative Container (PAC) -- Vertex to Vertex or Integer or Color (VV, VI, VC) */
  typedef std::map< Vertex, Vertex > PAC_VV_t; 
  typedef std::map< Vertex, int > PAC_VI_t; 
  typedef std::map< Vertex, default_color_type > PAC_VC_t; 
  /* Associative Property Map (APM) -- Vertex to Integer or Color (VV, VI, VC) */
  typedef boost::associative_property_map< PAC_VV_t > APM_VV_t;
  typedef boost::associative_property_map< PAC_VI_t > APM_VI_t;
  typedef boost::associative_property_map< PAC_VC_t > APM_VC_t;

  PAC_VI_t pac_component;
  PAC_VI_t pac_discover_time;
  PAC_VV_t pac_root;
  PAC_VC_t pac_color;
  
  APM_VI_t component_number ( pac_component );
  APM_VI_t discover_time ( pac_discover_time );
  APM_VV_t root ( pac_root );
  APM_VC_t color ( pac_color );
  
  unsigned int num_scc = strong_components(G, component_number, 
                                           root_map(root).
                                           color_map(color).
                                           discover_time_map(discover_time));

  std::vector < std::vector < Vertex > > components;
  build_component_lists(G, num_scc, component_number, components);

  /* DETECT SPURIOUS COMPONENTS */
  
  //   That is, any SCC which is a singleton not connecting to itself 
  //   is considered spurious and eliminated. Really we are not computing
  //   SCC but Path-SCC */
  
  // A vector to record which components are spurious.
  // "false" means a component is not spurious
  // "true" means a component is spurious
  std::vector<bool> spurious_components ( num_scc, false );
  unsigned int number_of_path_components = 0;
  
  // The following loop gives "spurious_components" the correct values:
  for ( unsigned int index = 0; index < num_scc; ++ index ) {
    if ( components [ index ] . size () == 1) {
      Vertex v = components [ index ] [ 0 ];
      if ( G(v) . find ( v ) == G(v) . end () ) {
        spurious_components [ index ] = true;
        continue;
      } /* if */
    } /* if */
    ++ number_of_path_components;
  }

  /* CONVERT RESULT INTO OUTPUT FORMAT */
  
  // First we size the result, and "new" the "Component"s, which are
  // "Toplex::Subset"s 
  morse_sets -> resize ( number_of_path_components );
  entrance_sets -> resize ( number_of_path_components );
  exit_sets -> resize ( number_of_path_components );
  
  // Now we copy the data 
  unsigned int comp_index = 0;
  unsigned int index = 0;
  BOOST_FOREACH ( std::vector<Vertex> & vertices, components ) {
    /* If the component is spurious, we do not record it. */
    if ( spurious_components [ index ++ ] ) continue;
    /* Otherwise, we copy it. */
    typename DirectedGraph< Toplex >::Component & morse_set = 
      ( * morse_sets ) [ comp_index ];
    typename DirectedGraph< Toplex >::Component & entrance_set = 
      ( * entrance_sets ) [ comp_index ];
    typename DirectedGraph< Toplex >::Component & exit_set = 
      ( * exit_sets ) [ comp_index ];
    ++ comp_index;
    BOOST_FOREACH ( typename Toplex::Top_Cell cell, vertices ) {
      morse_set . insert ( cell );
    } /* foreach */
    /* Entrance and Exit sets */
    // TODO: this is a trivial choice.
    entrance_set = morse_set;
    exit_set = morse_set;
  } /* foreach */
}

template < class Toplex, class Map >
DirectedGraph<Toplex> compute_directed_graph (const Toplex & my_toplex, 
                                              const Map & f) {
  DirectedGraph<Toplex> directed_graph;
  for (typename Toplex::const_iterator cell_iterator = my_toplex . begin ();
       cell_iterator != my_toplex . end (); ++ cell_iterator ) {
    directed_graph[*cell_iterator] = my_toplex.cover(f(my_toplex.geometry(cell_iterator)));
  } /* for_each */
  
  return directed_graph;
} /* compute_directed_graph */

template < class Toplex, class Map >
DirectedGraph<Toplex> compute_directed_graph (const typename Toplex::Subset & my_subset,
                                              const Toplex & my_toplex, 
                                              const Map & f) {
  DirectedGraph<Toplex> directed_graph;
  BOOST_FOREACH ( typename Toplex::Top_Cell cell, my_subset ) {
    directed_graph[cell] = my_toplex.cover(f(my_toplex.geometry(cell)),my_subset);
  } /* for_each */
  
  return directed_graph;
} /* compute_directed_graph */
  

#if 0
// Compute the length of the longest path from v to w
// assuming the graph is acyclic
template < class Toplex >
size_t computeLongestPathLength(
    DirectedGraph<Toplex> & G,
    typename DirectedGraph<Toplex>::Vertex v,
    typename DirectedGraph<Toplex>::Vertex w)
{

  typedef DirectedGraph<Toplex> Graph;
  typedef typename DirectedGraph<Toplex>::Vertex Vertex;
  std::map <Vertex, size_t> distance;

  // Iterators
  typename Graph::iterator graph_it;
  //typename Graph::Components::iterator comp_it;
  //typename Graph::Component::iterator vert_it;
  typename Toplex::Subset::iterator edge_it;

  // Initialize 'distance'
  graph_it = G.begin();
  while (graph_it != G.end()) {
    distance[(*graph_it).first] = 0;
    ++graph_it;
  }
  distance[v] = 1;

  // Main loop
  for (size_t i = 0; i < G.size() - 1; i++) {
    graph_it = G.begin();
    while (graph_it != G.end()) {
      if (distance[(*graph_it).first] > 0) {
        edge_it = ((*graph_it).second).begin();
        while (edge_it != ((*graph_it).second).end()) {
          if (distance[(*graph_it).first] + 1 > distance[*edge_it]) {
            distance[*edge_it] = distance[(*graph_it).first] + 1;
          }
          ++edge_it;
        }
      }
      ++graph_it;
    }
  }


  if (distance[w] > 0) {
    return distance[w] - 1;
  } else {
    return 0;
  }
} /* computeLongestPathLength */

// The main loop of 
template < class Toplex >
void BFLoop(std::map <typename DirectedGraph<Toplex>::Vertex, size_t> * in_distance,
            const DirectedGraph<Toplex> & G )
{
  /* Refer to output by reference */
  std::map <typename DirectedGraph<Toplex>::Vertex, size_t> & distance = * in_distance;
  /* Typedefs */
  typedef DirectedGraph<Toplex> Graph;
  typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
  typedef typename boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
  
  if ( G . size () == 0 ) return;  
  // Main loop
  /* Loop |G| - 1 times. */
  for (size_t i = 0; i < G.size() - 1; i++) {
    /* "Relax" every edge in the graph: */
    /* Loop through the vertices of G */
    vertex_iterator vert_it, vert_end;
    for (boost::tie ( vert_it, vert_end ) = boost::vertices ( G );
         vert_it != vert_end; ++ vert_it ) {
      if ( distance[ * vert_it ] > 0) {
        /* Loop through the adjacent vertices of *vert_it */
        adjacency_iterator adj_it, adj_end;
        for (boost::tie ( adj_it, adj_end ) = boost::adjacent_vertices ( *vert_it, G );
             adj_it != adj_end; ++ adj_it ) {
          if ( distance[ *vert_it ] + 1 > distance[ *adj_it]) {
            distance[ *adj_it ] = distance[ *vert_it ] + 1;
          } /* if */
        } /* for */
      } /* if */
    } /* for */
  } /* for */
  return;
} /* BFLoop */

template < class Toplex >
void computePathBounds(std::vector<size_t> * ConnectingPathBounds,
                       std::vector<size_t> * EntrancePathBounds,
                       std::vector<size_t> * ExitPathBounds,
                       size_t * ThruPathBound,
                       const DirectedGraph<Toplex> & G,
                       const typename DirectedGraph<Toplex>::Components & SCC,
                       const typename Toplex::Subset & Entrance,
                       const typename Toplex::Subset & Exit )
{

  typedef DirectedGraph<Toplex> Graph;
  typedef typename DirectedGraph<Toplex>::Vertex Vertex;
  typedef typename DirectedGraph<Toplex>::Components Components;
//  size_t n = G.size();
  size_t nComponents = SCC.size();
  typename Graph::iterator graph_it;
  typename Toplex::Subset::const_iterator comp_it;

  Graph H;
  std::vector<Vertex> V;

  // Compute the collapsed graph.
  // V is a vector containing vertices of the collapsed graph
  // corresponding to strongly connected components
  H = collapseComponents(G, SCC, V);
  
  // Components to Components
  ConnectingPathBounds -> clear();
  for (size_t i = 0; i < nComponents; i++) {
    for (size_t j = 0; j < nComponents; j++) {
      if (i == j) {
        ConnectingPathBounds -> push_back(0);
      } else {
        Graph K(H);
        for (size_t k = 0; k < nComponents; k++) {
          if ((k != i) & (k != j)) {
            
            K.removeVertex(V[k]);
          }
        }
        ConnectingPathBounds -> push_back(computeLongestPathLength(K, V[i], V[j]));
      }
    }
  }
  
  std::map <Vertex, size_t> distance;

  // Entrance to Components
  EntrancePathBounds -> clear();
  for (size_t i = 0; i < nComponents; i++) {
    Graph K(H);
    for (size_t j = 0; j < nComponents; j++) {
      if (j != i) {
        K.removeVertex(V[j]);
      }
    }
    distance.clear();
    graph_it = K.begin();
    while(graph_it != K.end()) {
      if (Entrance.count((*graph_it).first)) {
        distance[(*graph_it).first] = 1;
      } else {
        distance[(*graph_it).first] = 0;
      }
      ++graph_it;
    }
    BFLoop(&distance, K);
    EntrancePathBounds -> push_back(distance[V[i]]);
  }

  // Components to Exit
  ExitPathBounds -> clear();
  for (size_t i = 0; i < nComponents; i++) {
    Graph K(H);
    for (size_t j = 0; j < nComponents; j++) {
      if (j != i) {
        K.removeVertex(V[j]);
      }
    }
    distance.clear();
    graph_it = K.begin();
    while(graph_it != K.end()) {
      distance[(*graph_it).first] = 0;
      ++graph_it;
    }
    distance[V[i]] = 1;
    BFLoop(&distance, K);
    size_t maxLength = 0;
    comp_it = Exit.begin();
    while (comp_it != Exit.end()) {
      if (maxLength < distance[*comp_it]) {
        maxLength = distance[*comp_it];
      }
      ++comp_it;
    }
    ExitPathBounds -> push_back(maxLength);
  }

  // Entrance to Exit
  Graph K(H);
  for (size_t i = 0; i < nComponents; i++) {
    K.removeVertex(V[i]);
  }
  distance.clear();
  graph_it = K.begin();
  while(graph_it != K.end()) {
    if (Entrance.count((*graph_it).first)) {
      distance[(*graph_it).first] = 1;
    } else {
      distance[(*graph_it).first] = 0;
    }
    ++graph_it;
  }
  BFLoop(&distance, K);
  size_t maxLength = 0;
  comp_it = Exit.begin();
  while (comp_it != Exit.end()) {
    if (maxLength < distance[*comp_it]) {
      maxLength = distance[*comp_it];
    }
    ++comp_it;
  }
  *ThruPathBound = maxLength;
  
  return;
}

#endif

#if 1

// Compute the lengths of the longest paths beginning in a source set "Source"
// and terminating at any given vertex of the graph. (A result of zero means either
// a) the vertex was in the source or b) no such path exists)
template < class Toplex >
void LongestPathLengths(std::map <typename DirectedGraph<Toplex>::Vertex, size_t> * in_distance,
                        const typename Toplex::Subset Source,
                        const std::vector<typename DirectedGraph<Toplex>::Vertex> & top_sorted_vertices,
                        const DirectedGraph<Toplex> & G )
{
  
  /* Typedefs */
  typedef DirectedGraph<Toplex> Graph;
  typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
  typedef typename boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
  typedef 
  typename DirectedGraph<Toplex>::Vertex Vertex;
  std::map <Vertex, size_t> & distance = * in_distance;
  distance . clear ();
  
  /* Loop through the vertices of G in topologically sorted order */
  vertex_iterator vert_it, vert_end;
  
  /* Initialize the distance structure with 0 */
  BOOST_FOREACH ( Vertex v, top_sorted_vertices ) {
    distance [ v ] = 0;
  } /* boost_foreach */
  
  BOOST_FOREACH ( Vertex v, top_sorted_vertices ) {
    if ( distance [ v ] > 0 || Source . find ( v ) != Source . end () ) {    
      /* Loop through the adjacent vertices of v */
      adjacency_iterator adj_it, adj_end;
      for (boost::tie ( adj_it, adj_end ) = boost::adjacent_vertices ( v, G ); 
           adj_it != adj_end; ++ adj_it ) {
        if ( distance[ v ] + 1 > distance[ *adj_it]) distance[ *adj_it ] = distance[ v ] + 1;
      } /* for */
    } /* if */
  } /* for */
  
} /* LongestPathLengths */

template < class Toplex >
void computePathBounds(std::vector<size_t> * ConnectingPathBounds,
                       std::vector<size_t> * EntrancePathBounds,
                       std::vector<size_t> * ExitPathBounds,
                       size_t * ThruPathBound,
                       const DirectedGraph<Toplex> & G,
                       const typename DirectedGraph<Toplex>::Components & SCC,
                       const typename Toplex::Subset & Entrance,
                       const typename Toplex::Subset & Exit )
{
  using namespace boost;
  typedef DirectedGraph<Toplex> Graph;
  typedef typename DirectedGraph<Toplex>::Vertex Vertex;
  
  //debug
  /*
  typedef const std::pair < const typename Toplex::Top_Cell, typename Toplex::Subset > DG_value;

  BOOST_FOREACH ( DG_value & x, G ) {
    BOOST_FOREACH ( typename Toplex::Top_Cell target_cell, x . second ) {
      if ( G . find ( target_cell ) == G . end () ) std::cout << " G [ " << x . first << "] has " <<
        target_cell << " yet " << target_cell << " is not in the domain of G\n";
    }
  }
  */
  
  // Compute the collapsed graph.
  // V is a vector containing vertices of the collapsed graph
  // corresponding to strongly connected components
  std::vector<Vertex> V;
  Graph H = collapseComponents(G, SCC, V);
  
  /* Pair Associative Container (PAC) -- Vertex to Vertex or Integer or Color (VV, VI, VC) */
  //typedef std::map< Vertex, Vertex > PAC_VV_t; 
  //typedef std::map< Vertex, int > PAC_VI_t; 
  typedef std::map< Vertex, default_color_type > PAC_VC_t; 
  /* Associative Property Map (APM) -- Vertex to Integer or Color (VV, VI, VC) */
  //typedef boost::associative_property_map< PAC_VV_t > APM_VV_t;
  //typedef boost::associative_property_map< PAC_VI_t > APM_VI_t;
  typedef boost::associative_property_map< PAC_VC_t > APM_VC_t;
  
  //PAC_VI_t pac_component;
  //PAC_VI_t pac_discover_time;
  //PAC_VV_t pac_root;
  PAC_VC_t pac_color;
  
  //APM_VI_t component_number ( pac_component );
  //APM_VI_t discover_time ( pac_discover_time );
  //APM_VV_t root ( pac_root );
  APM_VC_t color ( pac_color );
  
  //DEBUG
  std::cout << "Topological sort about to begin on graph H with " << H . size () << " vertices\n";

  /*
  BOOST_FOREACH ( DG_value & x, H ) {
    BOOST_FOREACH ( typename Toplex::Top_Cell target_cell, x . second ) {
      if ( H . find ( target_cell ) == H . end () ) std::cout << " H [ " << x . first << "] = " <<
        target_cell << " yet " << target_cell << " is not in the domain of H\n";
    }
  }
   */
  
  std::vector<Vertex> top_sorted_vertices ( H . size () );
  boost::topological_sort(H, std::back_inserter ( top_sorted_vertices ), color_map(color));


  std::map <typename DirectedGraph<Toplex>::Vertex, size_t> distance;
  
  /* Initialize the lengths of the output vectors */
  EntrancePathBounds -> resize ( V . size () );
  ExitPathBounds -> resize ( V . size ());
  ConnectingPathBounds -> resize ( V . size () * V . size () );
  
  /* Compute lengths beginning from the Entrance set.
     From this information we can compute the 
       entrance -> components
       entrance -> exit
     path length upper bounds */
  LongestPathLengths ( &distance, Entrance, top_sorted_vertices, H );
  
  // Entrance to Components
  for ( size_t i = 0; i < V . size (); ++ i ) {
    (*EntrancePathBounds)[i] = distance [ V [ i ] ];
  } /* for */

  // Entrance to Exit
  *ThruPathBound = 0;
  BOOST_FOREACH ( Vertex v, Exit ) {
    if ( distance [ v ] > *ThruPathBound ) *ThruPathBound = distance [ v ];
  } /* boost_foreach */
  
  // Components to Components and Exit
  size_t index = 0;
  for ( size_t i = 0; i < V . size (); ++ i ) {
    typename Toplex::Subset source;
    source . insert ( V [ i ] );
    LongestPathLengths ( &distance, source, top_sorted_vertices, H );
    for ( size_t j = 0; j < V . size (); ++ j ) {
      (*ConnectingPathBounds) [ index ++ ] = distance [ V [ j ] ];
    } /* for */
    
    (*ExitPathBounds)[i] = 0;
    BOOST_FOREACH ( Vertex v, Exit ) {
      if ( distance [ v ] > (*ExitPathBounds)[i] ) (*ExitPathBounds)[i] = distance [ v ];
    } /* boost_foreach */
  } /* for */
  
} /* computePathBounds */

#endif

