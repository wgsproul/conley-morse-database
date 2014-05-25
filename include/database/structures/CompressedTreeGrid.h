//CompressedGrid.h
#ifndef CMDB_COMPRESSED_TREEGRID_H
#define CMDB_COMPRESSED_TREEGRID_H
#include <vector>
#include "database/structures/RectGeo.h"
#include "database/structures/CompressedTree.h"
#include "boost/shared_ptr.hpp"

class CompressedTreeGrid {
public:
  /// CompressedTreeGrid
  CompressedTreeGrid ( void );

  /// subdivide 
  void subdivide ( void );

  /// size
  size_t size ( void ) const;

  /// tree
  boost::shared_ptr<CompressedTree> & tree ( void );
  const boost::shared_ptr<CompressedTree> & tree ( void ) const;

  /// bounds
  RectGeo & bounds ( void );
  const RectGeo & bounds ( void ) const;

  /// dimension
  int dimension ( void ) const;

  /// periodic
  std::vector<bool> & periodicity ( void );
  const std::vector<bool> & periodicity ( void ) const;

private:
  RectGeo bounds_;
  std::vector < bool > periodicity_;
  boost::shared_ptr<CompressedTree> tree_;
};

inline 
CompressedTreeGrid::CompressedTreeGrid ( void ) {
  tree_ . reset ( new CompressedTree );
}

inline void 
CompressedTreeGrid::subdivide ( void ) {
  tree_ -> subdivide ();
}


inline size_t 
CompressedTreeGrid::size ( void ) const {
  return tree () -> leafCount ();
}

inline boost::shared_ptr<CompressedTree> &
CompressedTreeGrid::tree ( void ) {
  return tree_;
}

inline const boost::shared_ptr<CompressedTree> &
CompressedTreeGrid::tree ( void ) const {
  return tree_;
}

inline RectGeo &
CompressedTreeGrid::bounds ( void ) {
  return bounds_;
}

inline const RectGeo & 
CompressedTreeGrid::bounds ( void ) const {
  return bounds_;
}

inline int 
CompressedTreeGrid::dimension ( void ) const {
  return bounds () . dimension ();
}

inline std::vector<bool> & 
CompressedTreeGrid::periodicity ( void ) {
  return periodicity_;
}
inline const std::vector<bool> & 
CompressedTreeGrid::periodicity ( void ) const {
  return periodicity_;
}

#endif
