// RelativeMapHomology.h
// Shaun Harker
// 9/16/11

#ifndef CHOMP_RELATIVEMAPHOMOLOGY_H
#define CHOMP_RELATIVEMAPHOMOLOGY_H

#include <vector>
#include "boost/unordered_map.hpp"
#include "boost/unordered_set.hpp"
#include "chomp/Matrix.h"
#include "chomp/Generators.h"
#include "chomp/Toplex.h"
#include "chomp/Closure.h"
#include "chomp/FiberComplex.h"


#include "Draw.h"

#define PRINT if ( 1 ) std::cout <<

namespace chomp {

typedef std::vector < SparseMatrix < Ring > > RelativeMapHomology_t;

template < class Container, class RectMap > int 
RelativeMapHomology (RelativeMapHomology_t * output, 
                     const Toplex & source, 
                     const Container & XE, 
                     const Container & AE,
                     const Toplex & target, 
                     const Container & YE, 
                     const Container & BE,
                     const RectMap & F,
                     int depth ) {
  // Create the Relative Complexes
  RelativePair domain_pair;
  RelativePair codomain_pair;
  // Determine depth.
  // TODO: determine it, don't ask for it as input
  source . relativeComplex ( &domain_pair, XE, AE, depth );
  // TODO: don't repeat work here
  target . relativeComplex ( &codomain_pair, YE, BE, depth );
  
  PRINT "RMH: Constructed relative complexes\n";
  
  CubicalComplex & full_domain = domain_pair . base ();
  CubicalComplex & full_codomain = codomain_pair . base ();
  BitmapSubcomplex & domain = domain_pair . pair ();
  BitmapSubcomplex & codomain = codomain_pair . pair ();
  BitmapSubcomplex & DomainA = domain_pair . relative ();

  int D = full_domain . dimension ();
  
  /// Generate "domain_GridElements_X" and "domain_GridElements_A" tables
  /// These are for determining which top cells are involved in
  /// constructing a fiber (and which are relative)
  std::vector < std::vector < boost::unordered_set < Index > > > 
  domain_GridElements_X ( D + 1 );
  std::vector < std::vector < boost::unordered_set < Index > > > 
  domain_GridElements_A ( D + 1 );
  
  
  
  //PRINT "Computing domain grid elements \n";

  for ( int d = 0; d <= D; ++ d ) {
    domain_GridElements_X [ d ] . resize ( full_domain . size ( d ) );
    domain_GridElements_A [ d ] . resize ( full_domain . size ( d ) );
  }
  
  Index N;
  N = full_domain . size (D);
  for ( Index i = 0; i < N; ++ i ) {
    //PRINT " index = " << i << "\n";
    std::vector < boost::unordered_set < Index > > closure_of_i ( D + 1 );
    closure_of_i [ D ] . insert ( i );
    closure ( closure_of_i, full_domain );
    for ( int d = 0; d <= D; ++ d ) {
      //PRINT " d = " << d << "\n";
      BOOST_FOREACH ( Index j, closure_of_i [ d ] ) {
        domain_GridElements_X [ d ] [ j ] . insert ( i );
      }
    }
  }
  
  //PRINT "Computing domain grid elements 2\n";
  N = DomainA . size (D);
  for ( Index k = 0; k < N; ++ k ) {
    Index i = DomainA . indexToCell ( k, D );
    //PRINT " index = " << i << "\n";
    std::vector < boost::unordered_set < Index > > closure_of_i ( D + 1 );
    closure_of_i [ D ] . insert ( i );
    closure ( closure_of_i, full_domain );
    for ( int d = 0; d <= D; ++ d ) {
      //PRINT " d = " << d << "\n";
      BOOST_FOREACH ( Index j, closure_of_i [ d ] ) {
        domain_GridElements_A [ d ] [ j ] . insert ( i );
      }
    }
  }
  
  //PRINT "computed domain_GridElements_X and _A\n";


  // DEBUG //////////////////////////////////
//  for ( int d = 0; d <= D; ++ d ) {
//    std::cout << "The number of " << d << "-cells in domain_GridElements_X is " << 
//    domain_GridElements_X [ d ] . size () << "\n";
//    std::cout << "The number of " << d << "-cells in domain_GridElements_A is " << 
//    domain_GridElements_A [ d ] . size () << "\n";    
//  }
//  MorseComplex debugcomplex ( domain );
//  MorseSanity ( debugcomplex );
  //////////////////////////////////////////
               
  // Compute the homology generators
  PRINT "RMH: Computing Domain Generators with MorseGenerators\n";
  Generators_t domain_gen = MorseGenerators ( domain );
  PRINT "RMH: Computing Morse Complex of Codomain (codomain_morse)\n";
  MorseComplex codomain_morse ( codomain );
  PRINT "RMH: Computing Generators of codomain_morse\n";
  Generators_t codomain_morse_gen = SmithGenerators ( codomain_morse );
  // Compute the cycles projected into the codomain through the graph 
  std::vector < std::vector < Chain > > codomain_cycles ( D + 1 );
  
  //PRINT "Main Loop\n";
  // Loop through each domain generator
  for ( int d = 0; d <= D; ++ d ) {

    int number_of_domain_gen = domain_gen [ d ] . size ();
    codomain_cycles [ d ] . resize ( number_of_domain_gen );

    PRINT "RMH: Dimension = " << d << "\n";
    PRINT "RMH: Number of Generators at this dimension = " << number_of_domain_gen << "\n";

    for ( int gi = 0; gi < number_of_domain_gen; ++ gi ) {
      PRINT "RMH: gi = " << gi << "\n";
      

      
      // Lift the domain generator into the Relative Graph Complex
      Chain answer;
      answer . dimension () = d;
      std::vector < boost::unordered_map < Index, Chain > > graph_boundary ( D + 1 );
      //   Begin by "Chain Lift"
      PRINT "RMH: Chain Lift \n";
      // First, get the domain generator and include it into full_domain
      Chain domain_generator = domain . include ( domain_gen [ d ] [ gi ] . first );
      
#if 0
      if ( domain_generator () . size () > 1000 ) {
        std::cout << "Relative size = " << domain_pair . relative () . size () << "\n";
        ComplexVisualization * cv = new ComplexVisualization ( "Domain Generator Picture." );
        std::cout << "About to draw complex.\n";
        cv -> drawRelativeComplex ( domain_pair, 100, 200 );
        std::cout << "The complex was drawn.\n";
        std::cout << domain_generator . dimension () << "\n";
        cv -> drawChain ( full_domain, domain_generator, 200 );
        // explore a minute here
        cv -> explore ();
        delete cv;
      }
#endif 
      
      
      PRINT "RMH: Chain size = " << domain_generator () . size () << "\n";
      // Now lift it:
      BOOST_FOREACH ( const Term & t, domain_generator () ) {
        //std::cout << "Dealing with fiber (" << t . index () << ", " << d << ") (*)\n";
        //std::cout << "   Term is " << t << "\n";
        // Determine fiber
        boost::unordered_set < GridElement > X_nbs, A_nbs;
        
        //std::cout << "domain_GridElements_X [ " << d << " ] . size () = " << 
        //domain_GridElements_X [ d ] . size () << "\n";
        
        //std::cout << "domain_GridElements_A [ " << d << " ] . size () = " << 
        //domain_GridElements_A [ d ] . size () << "\n";
        
        X_nbs = domain_GridElements_X [ d ] [ t . index () ];
        A_nbs = domain_GridElements_A [ d ] [ t . index () ];    
        
        // DEBUG ///////////////////////
        //std::cout << "   X_nbs . size () = " << X_nbs . size () << "\n";
        //std::cout << "   A_nbs . size () = " << A_nbs . size () << "\n";
        ////////////////////////////////
        
        FiberComplex fiber ( X_nbs, A_nbs, full_domain, full_codomain, F );
        // Choose a vertex in fiber
        Index choice = fiber . choose ();
        if ( d == 0 ) answer += Term ( choice, t . coef () );
        Chain bd = full_domain . boundary ( t . index (), d );
        BOOST_FOREACH ( const Term & s, bd () ) {
          graph_boundary [d-1] [ s . index () ] . dimension () = 0;
          graph_boundary [d-1] [ s . index () ] += Term ( choice, t . coef () * s . coef () );
        }
      }
      PRINT "RMH: Cycle Lift -- iterative preboundary loop begins now\n";

      // Now process every fiber we can see in graph_boundary
      // Loop through every fiber dimension (fd)
      for ( int fd = d - 1; fd >= 0; -- fd ) {
        PRINT "  fiber dimension = " << fd << "\n";

        // Loop through every non-empty fiber
        typedef std::pair<Index,Chain> IndexChainPair;
        BOOST_FOREACH ( const IndexChainPair & fiberchain, graph_boundary [fd] ) {
          //std::cout << "Dealing with fiber (" << fiberchain . first << ", " << fd << ")\n";
          //std::cout << "   Chain is " << fiberchain . second << "\n";

          // Determine fiber
          boost::unordered_set < GridElement > X_nbs, A_nbs;
          X_nbs = domain_GridElements_X [ fd ] [ fiberchain . first ];
          A_nbs = domain_GridElements_A [ fd ] [ fiberchain . first ];    
          FiberComplex fiber ( X_nbs, A_nbs, full_domain, full_codomain, F );
          // Determine chain in fiber
          Chain projected = fiber . project ( fiberchain . second );
          // Work out the preboundary
          Chain preboundary = fiber . preboundary ( projected );
          // Include the preboundary back into the codomain
          Chain included_preboundary = fiber . include ( preboundary );

          // DEBUG///////////////////////////////////////////////////////////////////////
#if 0

          std::cout << "   A_nbs . size () = " << A_nbs . size () << "\n";
          std::cout << "   Projected Chain is " << projected << "\n";
          std::cout << "   Preboundary Chain is " << preboundary << "\n";
          std::cout << "   Included Chain is " << included_preboundary << "\n";

          Chain reboundary = fiber . boundary ( preboundary );
          std::cout << "   bd(preboundary) = " << simplify(reboundary) << "\n";

          Chain increboundary = full_codomain . boundary ( included_preboundary );
          std::cout << "   bd(included) = " << simplify(increboundary) << "\n";
          
          Chain leftover = simplify ( projected - reboundary );
          if ( not leftover () . empty () ) {
            std::cout << "   left over = " << simplify(projected - reboundary) << "\n";
            exit ( 1 );
          }

          MorseComplex morse ( fiber );
          std::cout << "\n Number of Cells = " << fiber . size () << ": ";
          for ( int dd = 0; dd <= fiber . dimension (); ++ dd ) 
            std::cout << fiber . size ( dd ) << " ";
          std::cout << "\n Number of Critical Cells = " << morse . size () << ": ";
          for ( int dd = 0; dd <= morse . dimension (); ++ dd ) 
            std::cout << morse . size ( dd ) << " ";
          
          {
          ComplexVisualization * cv = new ComplexVisualization ( "Fiber Picture." );
            std::cout << "About to draw complex.\n";
          cv -> drawComplex ( fiber, 100 );
            std::cout << "The complex was drawn.\n";
            std::cout << projected . dimension () << "\n";
          cv -> drawChain ( fiber, projected, 200 );
            std::cout << "The projected chain drew.\n";
          cv -> drawChain ( fiber, preboundary, 200 );
          // explore a minute here
          cv -> explore ();
          delete cv;
          }
#endif

          ///////////////////////////////////////////////////////////////////////////////
          
          // If this is a zero-dim fiber, add to answer
          if ( fd == 0 ) answer -= included_preboundary;
          // Add preboundary to adjacent fibers
          Chain bd = full_domain . boundary (fiberchain . first, fd );
          BOOST_FOREACH ( const Term & s, bd () ) {
            graph_boundary [fd-1] [ s . index () ] . dimension () = d - fd;
            graph_boundary [fd-1] [ s . index () ] -= included_preboundary * s . coef ();
          }
        }
      }

      //std::cout << "Projection to codomain.\n";
      // First project into the relative codomain complex
      Chain relative_answer = simplify ( codomain . project ( answer ) );
      
      // Project the Relative Graph Cycle to the Codomain
      codomain_cycles [ d ] [ gi ] = codomain_morse . lower ( relative_answer );
      
    }
  }
  PRINT "RMH: Final processing \n";

  // Loop through each Codomain Cycle
  for ( int d = 0; d <= codomain_morse . dimension (); ++ d ) {
    Matrix G = chainsToMatrix ( codomain_morse_gen [ d ], codomain_morse, d );
    Matrix Z = chainsToMatrix ( codomain_cycles [ d ], codomain_morse, d );
    Matrix MapHom = SmithSolve (G, Z);
    print_matrix (G);
    print_matrix (Z);
    print_matrix (MapHom);
    output -> push_back ( MapHom );
  }
  for ( int d = codomain_morse . dimension () + 1; d <= D; ++ d ) {
    output -> push_back ( Matrix (0, 0) );
  }
  
  PRINT "RMH: Returning.\n";
  return 0;
}
  
} // namespace chomp

#endif
