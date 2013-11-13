/*
 *  Conley_Index_Job.hpp
 */

#include "chomp/ConleyIndex.h"

#include "database/program/Configuration.h"
#include "database/program/jobs/Compute_Morse_Graph.h"
#include "database/structures/MorseGraph.h"
#include "database/structures/Database.h"
#include "database/structures/Grid.h"
#include "database/structures/PointerGrid.h"

#include <boost/thread.hpp>
#include <boost/chrono/chrono_io.hpp>

#include "chomp/Rect.h"
#include <vector>
#include <string>

#include "database/algorithms/conleyIndexString.h"

class ConleyIndexThread {

private:
  chomp::ConleyIndex_t * ci_matrix;
  boost::shared_ptr<Grid> phase_space;
  std::vector < Grid::GridElement > * subset;
  GeometricMap * map;
public:
  bool computed;
  ConleyIndexThread(  chomp::ConleyIndex_t * ci_matrix,
                      boost::shared_ptr<Grid> phase_space,
                      std::vector < Grid::GridElement > * subset,
                      GeometricMap * map) 
  : ci_matrix(ci_matrix), phase_space(phase_space), subset(subset), map(map), computed(false) {}
  void operator () ( void ) {
    try {
      ConleyIndex ( ci_matrix, *phase_space, *subset, *map );
      computed = true;
    } catch ( ... /* boost::thread_interrupted& */) {
      computed = false;
    }
  }
};


template <class PhaseGrid >
void Conley_Index_Job ( Message * result , const Message & job ) {
  using namespace chomp;
  // Read job
  size_t job_number;
  uint64_t incc;
  chomp::Rect geo;
  uint64_t ms;
  int PHASE_SUBDIV_INIT;
  int PHASE_SUBDIV_MIN;
  int PHASE_SUBDIV_MAX;
  int PHASE_SUBDIV_LIMIT;
  Rect PHASE_BOUNDS;
  std::vector < bool > PHASE_PERIODIC;
  job >> job_number;
  job >> incc;
  job >> geo;
  job >> ms;
  job >> PHASE_SUBDIV_INIT;
  job >> PHASE_SUBDIV_MIN;
  job >> PHASE_SUBDIV_MAX;
  job >> PHASE_SUBDIV_LIMIT;
  job >> PHASE_BOUNDS;
  job >> PHASE_PERIODIC;
  
  std::cout << "CIJ: job_number = " << job_number << "  (" << geo << ", " <<  ms << ")\n";

  // Compute Morse Graph
  MorseGraph mg;
  
  boost::shared_ptr<Grid> phase_space ( new PhaseGrid );
  phase_space -> initialize ( PHASE_BOUNDS, PHASE_PERIODIC );
  
     std::cout << "CIJ: geo = " << geo << "\n";
  
  GeometricMap map ( geo );

  std::cout << "CIJ: calling Compute_Morse_Graph\n";
  
  Compute_Morse_Graph ( &mg,
                        phase_space,
                        map,
                        PHASE_SUBDIV_INIT,
                        PHASE_SUBDIV_MIN,
                        PHASE_SUBDIV_MAX,
                        PHASE_SUBDIV_LIMIT );
  
    std::cout << "CIJ: returned from Compute_Morse_Graph\n";

  // Select Subset
    std::cout << "PHASE_PERIODIC = " << (PHASE_PERIODIC[0] ? "yes" : "no" ) << "\n";
    std::cout << "phase bounds = " << PHASE_BOUNDS << "\n";
    std::cout << "incc = " << incc << "\n";
    std::cout << "ms = " << ms << "\n";
    std::cout << "num vertices = " << mg . NumVertices () << "\n";

  CI_Data ci_data;
  if ( ms >= mg . NumVertices () ) {
    std::cerr << "Error: request to compute Conley Index for non-existence Morse Node.\n";
    abort ();
  }
  //if ( ms < mg.NumVertices () ) {

    std::cout << "CIJ: size of phase space = " << phase_space -> size () << "\n";
    std::cout << "CIJ: size of morse set = " << mg . grid ( ms ) -> size () << "\n";
    std::cout << "phase space grid type: " << typeid( * phase_space ).name() << "\n";
    std::cout << "ms grid type: " << typeid( * mg . grid ( ms ) ).name() << "\n";
  typedef std::vector < Grid::GridElement > Subset;
  Subset subset = phase_space -> subset ( * mg . grid ( ms ) );

  std::cout << "CIJ: calling Conley_Index on Morse Set " << ms << "\n";
  
  // Compute Conley Index Record of Morse Set
  ConleyIndex_t ci_matrix;

  // use a thread to perform the following line:    
  //      ConleyIndex ( & ci_matrix, *phase_space, subset, map );

  ConleyIndexThread cit ( &ci_matrix, phase_space, &subset, &map);
  boost::thread t(cit);
  if ( not t . try_join_for ( boost::chrono::seconds( 3600 ) ) ) {
   t.interrupt();
   t.join();
  }
  // end threading

  if ( cit . computed ) {
    std::cout << "CIJ: producing Conley Index polynomial strings \n";
    ci_data . conley_index = conleyIndexString ( ci_matrix );
  }
  if ( not cit . computed ) {
    ci_data . conley_index = std::vector<string> ();
    ci_data . conley_index . push_back ( "Relative Homology computation timed out.");
  }

  
  // Return Result
  * result << job_number;
  * result << incc;
  * result << ci_data;
  
}