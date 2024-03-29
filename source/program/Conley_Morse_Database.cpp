/*
 *  Conley_Morse_Database.cpp
 */

#define MAIN_CPP_FILE
#include "boost/serialization/export.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Model.h"

#include <ctime>
#include <fstream>
#define CMG_VERBOSE
#include "delegator/delegator.h"  /* For Coordinator_Worker_Scheme<>() */
#ifdef COMPUTE_MORSE_SETS
#include "database/program/MorseProcess.h"
#endif
#ifdef COMPUTE_CONTINUATION
#include "Model.h"
#include "database/structures/Database.h"
#endif
#ifdef COMPUTE_CONLEY_INDEX
#include "database/program/ConleyProcess.h"
#endif

#ifdef USE_SDSL
#include "database/structures/SuccinctGrid.h"
#endif
#include "database/structures/PointerGrid.h"
#include "database/structures/UniformGrid.h"
#include "database/structures/EdgeGrid.h"
 
#include <boost/serialization/export.hpp>
#ifdef USE_SDSL
BOOST_CLASS_EXPORT_IMPLEMENT(SuccinctGrid);
#endif
BOOST_CLASS_EXPORT_IMPLEMENT(PointerGrid);
BOOST_CLASS_EXPORT_IMPLEMENT(UniformGrid);
BOOST_CLASS_EXPORT_IMPLEMENT(EdgeGrid);


#include "database/structures/EuclideanParameterSpace.h"
#include "database/structures/AbstractParameterSpace.h"
BOOST_CLASS_EXPORT_IMPLEMENT(EuclideanParameter);
BOOST_CLASS_EXPORT_IMPLEMENT(EuclideanParameterSpace);
BOOST_CLASS_EXPORT_IMPLEMENT(AbstractParameterSpace);

int main ( int argc, char * argv [] ) {
  delegator::Start ();
#ifdef COMPUTE_MORSE_SETS
  std::cout << "STARTING MORSE PROCESS\n";
  time_t morsestarttime = time ( NULL );
  delegator::Run < MorseProcess > (argc, argv);
  time_t morsetime = time ( NULL ) - morsestarttime;
  {
  std::string filestring ( argv[1] );
  std::string appendstring ( "/MorseProcessTime.txt" );
  std::ofstream morsetimeoutfile ( (filestring + appendstring) . c_str () );
  morsetimeoutfile << morsetime << " seconds.\n";
  morsetimeoutfile . close ();
  }
#endif

#ifdef COMPUTE_CONTINUATION
  std::cout << "STARTING CONTINUATION PROCESS\n";
  int comm_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  if ( comm_rank == 0 ) {
    Database database;
    {
    std::string filestring ( argv[1] );
    std::string appendstring ( "/database.raw" );
    database . load ( (filestring + appendstring) . c_str () );
    }
    //database . removeBadBoxes<ModelMap> ();
    database . postprocess ();
    {
    std::string filestring ( argv[1] );
    std::string appendstring ( "/database.mdb" );
    database . save ( (filestring + appendstring) . c_str () );
    }
  }
#endif

#ifdef COMPUTE_CONLEY_INDEX  
  std::cout << "STARTING CONLEY PROCESS\n";
  time_t conleystarttime = time ( NULL );
  delegator::Run < ConleyProcess > (argc, argv);
  time_t conleytime = time ( NULL ) - conleystarttime;
  {
  std::string filestring ( argv[1] );
  std::string appendstring ( "/ConleyProcessTime.txt" );
  std::ofstream conleytimeoutfile ( (filestring + appendstring) . c_str () );
  conleytimeoutfile << conleytime << " seconds.\n";
  conleytimeoutfile . close ();
  }
#endif
  delegator::Stop ();


  return 0;
}
