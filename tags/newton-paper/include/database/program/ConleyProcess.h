#ifndef CMDB_CONLEYPROCESS_H
#define CMDB_CONLEYPROCESS_H

#include "delegator/delegator.h"
#include "database/structures/Database.h"
#include "database/program/Configuration.h"

#include <vector>
#include "database/structures/Grid.h"

/* * * * * * * * * * * * * * */
/* ConleyProcess declaration */
/* * * * * * * * * * * * * * */
class ConleyProcess : public Coordinator_Worker_Process {
public:
  void initialize ( void );
  int  prepare ( Message & job );
  void work ( Message & result, const Message & job ) const;
  void accept ( const Message &result );
  void finalize ( void ); 
private:
  size_t num_jobs_;
  size_t num_jobs_sent_;

  Configuration config;
  Database database;
  std::set<uint64_t> progress_detail;
  std::vector<int> attempts;
  uint64_t last_job_sent;
  int progress_bar; // progress bar
  clock_t time_of_last_checkpoint;
  clock_t time_of_last_progress;
};

#endif