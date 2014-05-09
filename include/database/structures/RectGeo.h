// RectGeo.h
// Shaun Harker
// 9/8/13

#ifndef CHOMP_RECTGEO_H
#define CHOMP_RECTGEO_H

#include <iostream>
#include <vector>
#include <cassert>

#include <boost/functional/hash.hpp>
#include "boost/serialization/serialization.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/foreach.hpp"

#include "database/structures/Geo.h"

#ifndef MISSING_CHOMP
#include "chomp/Rect.h"
#endif

/***********
 * RectGeo *
 ***********/

#include "database/numerics/Real.h"

class RectGeo : public Geo {
public:

  std::vector < Real > lower_bounds;
  std::vector < Real > upper_bounds;

#ifndef MISSING_CHOMP
  // chomp::Rect conversions
  operator chomp::Rect ( void ) const {
    chomp::Rect output;
    output . lower_bounds = lower_bounds;
    output . upper_bounds = upper_bounds;
    return output;
  }

  RectGeo ( const chomp::Rect & input ) {
    lower_bounds = input . lower_bounds;
    upper_bounds = input . upper_bounds;
  }
#endif

  RectGeo ( void ) {};
  RectGeo ( unsigned int size ) { lower_bounds . resize ( size );
                                upper_bounds . resize ( size ); }
  unsigned int dimension ( void ) const {
    return lower_bounds . size ();
  }
  RectGeo ( unsigned int size, const Real & value ) 
  { lower_bounds . resize ( size, value );
    upper_bounds . resize ( size, value ); }
  RectGeo ( unsigned int size, const Real & lower_value, const Real & upper_value )
  { lower_bounds . resize ( size, lower_value );
    upper_bounds . resize ( size, upper_value ); }
  RectGeo ( unsigned int size, const std::vector<Real> & lower_values, const std::vector<Real> & upper_values )
  { assert(size == lower_values.size());
    assert(size == upper_values.size());
    lower_bounds = lower_values;
    upper_bounds = upper_values; }
  RectGeo ( const std::vector<Real> & point ) {
    lower_bounds = point;
    upper_bounds = point;
  }
  
  void init_from_point ( const std::vector<Real> & point ) {
    lower_bounds = point;
    upper_bounds = point;
  }

  bool intersects ( const RectGeo & other ) const;
private: 
  friend class boost::serialization::access; 
  template<class Archive>
  void save(Archive & ar, const unsigned int version) const
  {
    unsigned int size = lower_bounds . size ();
    ar & size;
    BOOST_FOREACH ( Real x, lower_bounds ) ar & x;
    BOOST_FOREACH ( Real x, upper_bounds ) ar & x;
  }
  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    unsigned int size;
    ar & size;
    lower_bounds . resize ( size );
    upper_bounds . resize ( size );
    for ( unsigned int index = 0; index < size; ++ index ) {
      ar & lower_bounds [ index ];
    } /* for */
    for ( unsigned int index = 0; index < size; ++ index ) {
      ar & upper_bounds [ index ];
    } /* for */
    
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

inline RectGeo operator * ( double scalar, const RectGeo & rhs ) {
    RectGeo result = rhs;
    int d = rhs . dimension ();
    for ( int i = 0; i < d; ++ i ) {
      result . lower_bounds [ i ] *= scalar;
      result . upper_bounds [ i ] *= scalar;
    }
    return result;
  }
  
inline RectGeo operator + ( const RectGeo & lhs, const RectGeo & rhs ) {
    // SHOULD THROW
    RectGeo result = lhs;
    int d = rhs . dimension ();
    for ( int i = 0; i < d; ++ i ) {
      result . lower_bounds [ i ] += rhs . lower_bounds [ i ];
      result . upper_bounds [ i ] += rhs . upper_bounds [ i ];
    }
    return result;
  }
  
inline std::ostream & operator << ( std::ostream & output_stream, const RectGeo & print_me );

  // We cast to float, assuming that == testing is for hashing
  inline bool operator==(RectGeo const& x, RectGeo const& y) {
    for ( size_t d = 0; d < x . dimension (); ++ d ) {
      if ( (float) x . lower_bounds [ d ] != (float) y . lower_bounds [ d ] ) return false;
      if ( (float) x . upper_bounds [ d ] != (float) y . upper_bounds [ d ] ) return false;
    }
    return true;
  }
  
inline std::size_t hash_value(RectGeo const& x)
  {
    std::size_t seed = 0;
    for ( size_t d = 0; d < x . dimension (); ++ d ) {
      boost::hash_combine(seed, (float) x . lower_bounds [ d ] );
      boost::hash_combine(seed, (float) x . upper_bounds [ d ] );
    }
    return seed;
  }
  
///////////// Definitions

// really bad temporary solution
#define TOL 1e-8 

inline bool RectGeo::intersects ( const RectGeo & other ) const {
  for ( unsigned int dimension_index = 0; 
        dimension_index < lower_bounds . size (); 
        ++ dimension_index ) {
    if ( upper_bounds [ dimension_index ] + TOL < 
         other . lower_bounds [ dimension_index ] ||
        other . upper_bounds [ dimension_index ] + TOL < 
        lower_bounds [ dimension_index ] ) {
      return false;
    } /* if */
  } /* for */
  return true;
}

inline std::ostream & operator << ( std::ostream & output_stream, const RectGeo & print_me ) {
  for ( unsigned int dimension_index = 0; dimension_index < print_me . lower_bounds . size (); ++ dimension_index ) {
    output_stream << "[" << print_me . lower_bounds [ dimension_index ] << ", " << print_me . upper_bounds [ dimension_index ] << "]";
    if ( dimension_index < print_me . lower_bounds . size () - 1 ) output_stream << "x";
  }
  return output_stream;
} 


#endif
