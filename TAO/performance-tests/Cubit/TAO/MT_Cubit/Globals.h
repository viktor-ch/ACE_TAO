// -*- C++ -*-

#ifndef GLOBALS_H
#define GLOBALS_H

//=============================================================================
/**
 *  @file    Globals.h
 *
 *  All the globally defined classes, functions, types, and #defines
 *  are centralized here.
 *
 *  @author Nagarajan Surendran <naga@cs.wustl.edu>
 */
//=============================================================================


#include "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Get_Opt.h"
#include "ace/Sched_Params.h"
#include "ace/Singleton.h"
#include "ace/Condition_Thread_Mutex.h"

#include "tao/orbconf.h"

#if defined (VXWORKS) && defined (VME_DRIVER)
#define VX_VME_INIT \
STATUS status = vmeDrv ();\
if (status != OK)\
     ACE_DEBUG ((LM_DEBUG,\
                "ERROR on call to vmeDrv()\n"));\
  status = vmeDevCreate ("/vme");\
  if (status != OK)\
    ACE_DEBUG ((LM_DEBUG,\
                "ERROR on call to vmeDevCreate()\n"));
#else
#define VX_VME_INIT
#endif /* VXWORKS && VME_DRIVER */

#if defined (__Lynx__)
#define SCHED_PRIORITY 30
#elif defined (VXWORKS)
#define SCHED_PRIORITY 6
#elif defined (ACE_WIN32) || defined (__FreeBSD__)
#define SCHED_PRIORITY \
ACE_Sched_Params::priority_max(ACE_SCHED_FIFO,ACE_SCOPE_THREAD)
#elif defined (HPUX)
#define SCHED_PRIORITY ACE_THR_PRI_FIFO_MAX
#else
#define SCHED_PRIORITY \
ACE_THR_PRI_FIFO_DEF + 25
#endif /* ! __Lynx__ */

#if defined (VXWORKS) && defined (FORCE_ARGS)
static char *force_argv[]=
{
  "server",
  "-f",
  "ior.txt"
};
#endif /* defined (VXWORKS) && defined (FORCE_ARGS) */

#if defined (VXWORKS) && defined (FORCE_ARGS)
#define FORCE_ARGV(argc,argv) \
argc = 4;
argv = force_argv;
#else /* !VXWORKS && !FORCE_ARGS */
#define FORCE_ARGV(argc,argv)
#endif
// Number of utilisation computations to compute the duration of one
// util computation.
#define NUM_UTIL_COMPUTATIONS 10000

#define THREAD_PER_RATE_OBJS 4
// Number of cubit objects in the thread per rate test.

#define TASK_HANDLE_LEN 32
// length of the task handle ,used in vxworks.

#define TASKNAME_LEN 14
// Length of the task name in the task control block for vxworks.

#define UTIL_BOUND_CONSTANT 1000
// A constant to avoid the utility thread blocking the machine and to
// bound its  number of computations.

ACE_BEGIN_VERSIONED_NAMESPACE_DECL
class ACE_Barrier;
ACE_END_VERSIONED_NAMESPACE_DECL

/**
 * @class Globals
 *
 * @brief Globals class to be used as a singleton.
 *
 * This is used both by the server and client.
 */
class Globals
{
public:
  /// default constructor.
  Globals ();

  /// parse the arguments.
  int parse_args (int argc, ACE_TCHAR **argv);

  /**
   * Enables fifo scheduling eg., RT scheduling class on solaris.
   * Returns 0 on success, 1 if insufficient permission, or -1
   * for other failure.  As a side effect, sets thr_create_flags
   * appropriately.
   */
  static int sched_fifo_init ();

  /// Thread creation flags.  Must call sched_fifo_init () before
  /// accessing.
  long thr_create_flags;

  /// Default thread priority, used for the high thread priority.
  /// Must call sched_fifo_init () before accessing.
  int default_priority;

  /// endpoint to be used for ORB_init.
  ACE_TCHAR endpoint[BUFSIZ];

  /// file name to read/write the iors of the servants.
  ACE_TCHAR *ior_file;

  /// number of objects per servant thread.
  u_int num_of_objs;

  ///  thread_per_rate test flag.
  u_int thread_per_rate;

  /// flag to use multiple priorities for the low priority servants
  /// instead of one priority.
  u_int use_multiple_priority;

  /// ready flag used by the high priority thread to wake up the low
  /// priority threads after it's parsed the arguments.
  int ready_;

  /// mutex for the condition variable.
  TAO_SYNCH_MUTEX ready_mtx_;

  /// condition variable for the low priority threads to wait
  ///until the high priority thread is done with the arguments parsing.
  TAO_SYNCH_CONDITION ready_cnd_;

  /// Barrier for the multiple servants to synchronize after binding to
  /// the orb.
  ACE_Barrier *barrier_;
};

// Make the globals a Singleton.
typedef ACE_Singleton<Globals,ACE_Null_Mutex> GLOBALS;

/**
 * @class MT_Priority
 *
 * @brief Helper class to find high and low priorities for the
 * MT_Cubit application over multiple platforms.
 *
 * This class has 2 methods, one for the high priority and the
 * other for low priority.If the flag use_multiple_priority is
 * passed then multiple priorities are used for the low priority
 * threads.
 */
class MT_Priority
{
public:
  /// constructor.
  MT_Priority ();

  /// Destructor.
  virtual ~MT_Priority ();

  /// Sets the priority of the high priority thread.
  virtual ACE_Sched_Priority get_high_priority ();

  /// Sets the priority to be used for the low priority thread.
  virtual ACE_Sched_Priority get_low_priority
    (u_int num_low_priority,
     ACE_Sched_Priority prev_priority,
     u_int use_multiple_priority);

  /// Accessor for num_priorities_.
  u_int number_of_priorities ();

  /// Accessor for grain_.
  u_int grain ();

protected:
  /// Number of priorities used.
  u_int num_priorities_;

  /**
   * Granularity of the assignment of the priorities.  Some OSs have
   * fewer levels of priorities than we have threads in our test, so
   * with this mechanism we assign priorities to groups of threads
   * when there are more threads than priorities.
   */
  u_int grain_;
};

#endif /* GLOBALS_H */
