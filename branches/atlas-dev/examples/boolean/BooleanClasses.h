#ifndef BOOLEANCLASSES_H
#define BOOLEANCLASSES_H

#include <boost/python.hpp>
#include <string>
#include <sstream>

// Simple Box with the two sets of parameters
class BooleanBox {
public:
	BooleanBox ( void ) {}
	BooleanBox ( chomp::Rect g, chomp::Rect s, chomp::Rect r ) : gamma(g),sigma(s),rect(r) { }

	chomp::Rect gamma;
	chomp::Rect sigma;
	chomp::Rect rect;
};

std::ostream& operator<< (std::ostream& os, const BooleanBox& box) {
	os << "Boolean Box with bounds : " << box.rect << " and parameters (gi,si) : ";
  os << box.gamma << " , " << box.sigma;
	os << "\n";
  return os;
}

class Face { // Will only work for cubes
public: 
  int direction; // means X_{direction} = cst. direction=-1 means a fixed point
  chomp::Rect rect; // will be n-dimensional, and degenerated along the dimension given by direction
};

std::ostream& operator<< (std::ostream& os, const Face& face) {
  os << "Face for x=cst in the direction : " << face.direction << " , ";
  os << " with bounds : " << face.rect << "\n";
  return os;
}

// need to know when two faces overlap 
bool intersect ( const chomp::Rect & rect1, const chomp::Rect & rect2 ) {
  int dim = rect1 . dimension();
  for ( unsigned int i=0; i<dim; ++i ) { 
    if ( ( rect1.lower_bounds[i] >= rect2.upper_bounds[i] ) || 
         ( rect1.upper_bounds[i] <= rect2.lower_bounds[i] ) ) {
      return false;
    } 
  }
  return true;
}

// Check when two faces are identical
bool operator == ( const Face & face1, const Face & face2 ) {
	double eps_ = 1e-10;	
		if ( face1 . direction != face2 . direction ) {
			return false;
		}  
		std::vector < double > lb1, ub1, lb2, ub2;
		lb1 = face1 . rect . lower_bounds;
		ub1 = face1 . rect . upper_bounds;
		lb2 = face2 . rect . lower_bounds;
		ub2 = face2 . rect . upper_bounds;
		for ( unsigned int i=0; i<face1.rect.dimension(); ++i ) {
			if ( (std::abs(lb1[i]-lb2[i]) > eps_) || (std::abs(ub1[i]-ub2[i]) > eps_) ) { return false; }
		} 
		return true;
	}


// Class for the configuration of a Boolean network and its definition
class BooleanConfig { 

public:
	void load ( const char * inputfile );

	std::vector < BooleanBox > listboxes ( void ) const { return booleanboxes_; }
  std::vector < BooleanBox > & listboxes ( void ) { return booleanboxes_; }

	chomp::Rect phasespace ( void ) const { return phasespace_; }



private:
	int dimension_;
	chomp::Rect phasespace_;
	std::vector < BooleanBox > booleanboxes_;
};


// Valid in Higher Dimension
inline void BooleanConfig::load ( const char * inputfile ) {
  using boost::property_tree::ptree;
  ptree pt;

  
  std::ifstream input ( inputfile );
  read_xml(input, pt);
//
  dimension_ = pt.get<int>("atlas.dimension");
//
  std::vector < double > lbgamma, ubgamma, lbsigma, ubsigma;
  std::vector < chomp::Rect > boxes;
//
  // Phase space information
  std::vector < double > lbounds, ubounds;
  lbounds . resize ( dimension_ );
  ubounds . resize ( dimension_ );
  std::string lower_bounds = pt.get<std::string>("atlas.phasespace.bounds.lower");
  std::string upper_bounds = pt.get<std::string>("atlas.phasespace.bounds.upper");
  std::stringstream lbss ( lower_bounds );
  std::stringstream ubss ( upper_bounds );
  for ( int d = 0; d < dimension_; ++ d ) {
    lbss >> lbounds [ d ];
    ubss >> ubounds [ d ];
  }
  phasespace_ = chomp::Rect ( dimension_, lbounds, ubounds );

  lbgamma . resize ( dimension_ );
  ubgamma . resize ( dimension_ );
  lbsigma . resize ( dimension_ );
  ubsigma . resize ( dimension_ );

  std::string gammastr = pt.get<std::string>("atlas.gamma.lower");
  std::stringstream gammass ( gammastr );
  std::string gammastr2 = pt.get<std::string>("atlas.gamma.upper");
  std::stringstream gammass2 ( gammastr2 );
 
  for ( int d = 0; d < dimension_; ++ d ) {
  	gammass >> lbgamma [ d ];
    gammass2 >> ubgamma [ d ];
  }

  chomp::Rect gamma ( dimension_, lbgamma, ubgamma );

  //
  BOOST_FOREACH ( ptree::value_type & v, pt.get_child("atlas.listboxes") ) {
		std::string sigmastr = v . second . get_child ( "sigma.lower" ) . data ( );
    std::string sigmastr2 = v . second . get_child ( "sigma.upper" ) . data ( );

    std::string lbstr = v . second . get_child ( "bounds.lower" ) . data ( );
    std::string ubstr = v . second . get_child ( "bounds.upper" ) . data ( );
    //
		std::stringstream sigmass ( sigmastr );
    std::stringstream sigmass2 ( sigmastr2 );

    std::stringstream lbss ( lbstr );
    std::stringstream ubss ( ubstr );
    //
		for ( int d = 0; d < dimension_; ++ d ) {
  		sigmass >> lbsigma [ d ];
      sigmass2 >> ubsigma [ d ];
  	}
  	//
    for ( unsigned int d = 0; d < dimension_; ++ d ) {
      lbss >> lbounds [ d ];
      ubss >> ubounds [ d ];
    }
    //
    chomp::Rect sigma ( dimension_, lbsigma, ubsigma );
    booleanboxes_ . push_back ( BooleanBox ( gamma, sigma, chomp::Rect(dimension_, lbounds, ubounds) ) );
  }
//
}




#endif