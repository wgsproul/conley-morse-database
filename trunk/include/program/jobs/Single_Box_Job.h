/*
 *  Single_Box_Job.h
 */

#ifndef _CMDP_SINGLE_BOX_JOB_
#define _CMDP_SINGLE_BOX_JOB_

#include "distributed/Messaging.h"

/// Computes the Conley-Morse decomposition with respect to the given map
/// on the given phase space. The toplex representation of the phase space
/// is subdivided the given number of times.
/// The Conley-Morse graph is stored in the data structure provided
/// (which must be initially empty), together with Morse sets
/// for which memory is dynamically allocated.
template < class Toplex , class Parameter_Toplex ,
  class Map , class Conley_Index , class Cached_Box_Information >
void Compute_Conley_Morse_Graph ( ConleyMorseGraph < typename Toplex::Toplex_Subset, Conley_Index > * conley_morse_graph ,
  const typename Parameter_Toplex::Geometric_Description & parameter_box ,
  Toplex * phase_space ,
  const typename Toplex::Geometric_Description & phase_space_box ,
  int subdivisions ,
  Cached_Box_Informatin * cached_box_information );

/// Constructs the Conley-Morse decomposition for a single parameter box.
/// Runs a complete single box job as one of the worker's tasks.
/// Reads the computation parameters from the job message.
/// Saves the results of the computation in the result message.
template < class Toplex, class Parameter_Toplex ,
  class Map , class Conley_Index , class Cached_Box_Information >
void Single_Box_Job ( Message * result, const Message & job );

#ifndef _DO_NOT_INCLUDE_HPP_
#include "program/jobs/Single_Box_Job.hpp"
#endif

#endif