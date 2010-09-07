/*
 *  Conley_Morse_Graph.h
 */

#ifndef _CMDP_CONLEY_MORSE_GRAPH_
#define _CMDP_CONLEY_MORSE_GRAPH_

#include <boost/graph/adjacency_list.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

/** Conley-Morse Graph.
 *  It is the directed acyclic graph whose vertices have a
 *  Cubeset and a ConleyIndex.
 *
 *  Note that this class doesn't validate acyclic condition.
 */
template<class CubeSet, class ConleyIndex>
class ConleyMorseGraph {
 private:
  struct Component;

 public:
  /* property of vertices and graph type */
  struct ComponentProperty {
    typedef boost::vertex_property_tag kind;
  };
  typedef boost::property<ComponentProperty, Component> VertexProperty;
  typedef boost::no_property EdgeProperty;
  typedef boost::adjacency_list<boost::listS, boost::listS,
                                boost::bidirectionalS,
                                VertexProperty, EdgeProperty> Graph;

  /* vertex identifier type and edge identifier type */
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  /* iterator types */
  typedef typename boost::graph_traits<Graph>::vertex_iterator VertexIterator;
  typedef std::pair<VertexIterator, VertexIterator> VertexIteratorPair;
  typedef boost::transform_iterator<
    boost::function<Vertex (Edge)>,
    typename boost::graph_traits<Graph>::out_edge_iterator> OutEdgeIterator;
  typedef std::pair<OutEdgeIterator, OutEdgeIterator> OutEdgeIteratorPair;
  typedef boost::transform_iterator<
    boost::function<Vertex (Edge)>,
    typename boost::graph_traits<Graph>::in_edge_iterator> InEdgeIterator;
  typedef std::pair<InEdgeIterator, InEdgeIterator> InEdgeIteratorPair;
  /* size types */
  typedef typename boost::graph_traits<Graph>::vertices_size_type
      VerticesSizeType;
  
  /** Create an empty graph */
  ConleyMorseGraph() {
    component_accessor_ = boost::get(ComponentProperty(), graph_);
  }
  /** Destruct a graph.
   *  Note that CubeSet and ConleyIndex assigned to each vertex are not deleted.
   */
  ~ConleyMorseGraph() {}
  /** Create a new vertex and return the descriptor of the vertex.
   *  The vertex is not connected to anywhere.
   *  The vertex is not related to any cube set and conley index
   *  just after this function is called.
   */
  Vertex AddVertex() {
    return boost::add_vertex(graph_);
  }
  /** Remove a vertex from the graph.
   *  All edges connected to and from the removed vertex are also removed.
   * 
   *  Note that CubeSet and ConleyIndex assigned to the vertex are not deleted,
   *  therefore please be careful not to leak memories.
   */
  void RemoveVertex(Vertex vertex) {
    boost::clear_vertex(vertex, graph_);
    boost::remove_vertex(vertex, graph_);
  }
  /** Add a edge from the "from" vertex to the "to" vertex.
   */
  void AddEdge(Vertex from, Vertex to) {
    boost::add_edge(from, to, graph_);
  }
  /** Return true if there is a path from the "from" vertex
   *  to the "to" vertex.
   *
   */
  bool PathExist(Vertex from, Vertex to);
  
  /** Get a cubeset of the vertex. */
  CubeSet* GetCubeSet(Vertex vertex) const {
    return component_accessor_[vertex].cube_set_;
  }
  /** Set a cubeset to the vertex */
  void SetCubeSet(Vertex vertex, CubeSet* cubeset) {
    component_accessor_[vertex].cube_set_ = cubeset;
  }
  /** Get a Conley-Index of the vertex. */
  ConleyIndex* GetConleyIndex(Vertex vertex) const {
    return component_accessor_[vertex].conley_index_;
  }
  /** Set a Conley-Index to the vertex */
  void SetConleyIndex(Vertex vertex, ConleyIndex* conley_index) {
    component_accessor_[vertex].conley_index_ = conley_index;
  }
  /** return a iterator pair to all vertices */
  VertexIteratorPair Vertices() const {
    return boost::vertices(graph_);
  }
  
  /** return a number of vertices */
  VerticesSizeType NumVertices() const {
    return boost::num_vertices(graph_);
  }
  
  /** return a iterator pair to all out-edges */
  OutEdgeIteratorPair OutEdges(Vertex vertex);
  
  /** return a iterator pair to all verteces of in-edges */
  InEdgeIteratorPair InEdges(Vertex vertex);
  
 private:
  Graph graph_;
  typename boost::property_map<Graph, ComponentProperty>::type component_accessor_;

  /** Return a target of given edge.
   *  This function is used in OutEdges() to avoid a compilation error
   *  of imcomplete type inference.
   */
  Vertex Target(Edge e) {
    return boost::target(e, graph_);
  }
  /** Return a source of given edge.
   *  This function is used in InEdges() for the same reason of Target().
   */
  Vertex Source(Edge e) {
    return boost::source(e, graph_);
  }

  /** struct of each component, which has a pointer to cubeset and conley index.
   *  there exist this struct because of serialization problem.
   */
  struct Component {
    CubeSet *cube_set_;
    ConleyIndex *conley_index_;
    Component() {
      cube_set_ = NULL;
      conley_index_ = NULL;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
      ar & conley_index_;
    }
  };
  
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & graph_;
  }
};

#ifndef _DO_NOT_INCLUDE_HPP_
#include "data_structures/Conley_Morse_Graph.hpp"
#endif

#endif