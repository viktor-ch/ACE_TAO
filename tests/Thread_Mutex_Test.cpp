// $Id$

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    Thread_Mutex_Test.cpp
//
// = DESCRIPTION
//      This is a simple test to illustrate the functionality of
//      ACE_Thread_Mutex. The test acquires and releases mutexes. No
//      command line arguments are needed to run the test.
//
// = AUTHOR
//    Prashant Jain <pjain@cs.wustl.edu> and Doug Schmidt <schmidt@cs.wustl.edu>
//
// ============================================================================

#include "test_config.h"
#include "ace/Thread_Manager.h"

ACE_RCSID(tests, Thread_Mutex_Test, "$Id$")

#if defined (ACE_HAS_THREADS)
static void *
test (void *args)
{
  ACE_Thread_Mutex *mutex = (ACE_Thread_Mutex *) args;
  ACE_UNUSED_ARG (mutex); // Suppress ghs warning about unused local "mutex".
  ACE_OS::srand (ACE_OS::time (0));

  for (size_t i = 0; i < ACE_MAX_ITERATIONS / 2; i++)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%P|%t) = trying to acquire on iteration %d\n"),
                  i));
      int result = mutex->acquire ();
      ACE_ASSERT (result == 0);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%P|%t) = acquired on iteration %d\n"),
                  i));

      // Sleep for a random amount of time between 0 and 2 seconds.
      // Note that it's ok to use rand() here because we are running
      // within the critical section defined by the Thread_Mutex.
      ACE_OS::sleep (ACE_OS::rand () % 2);

      result = mutex->release ();
      ACE_ASSERT (result == 0);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%P|%t) = released on iteration %d\n"),
                  i));
    }

  return 0;
}
#endif /* ACE_HAS_THREADS */

static void
spawn (void)
{
#if defined (ACE_HAS_THREADS)
  ACE_Thread_Mutex mutex;

  const u_int n_threads =
#if defined (__Lynx__)
    3;  /* It just doesn't work with 4 threads. */
#else  /* ! __Lynx__ */
    ACE_MAX_THREADS;
#endif /* ! __Lynx_- */

  if (ACE_Thread_Manager::instance ()->spawn_n (n_threads,
                                                ACE_THR_FUNC (test),
                                                (void *) &mutex,
                                                THR_NEW_LWP | THR_DETACHED) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p\n%a"),
                ACE_TEXT ("thread create failed")));

  // Wait for the threads to exit.
  ACE_Thread_Manager::instance ()->wait ();

#else
  ACE_ERROR ((LM_INFO,
              ACE_TEXT ("threads not supported on this platform\n")));
#endif /* ACE_HAS_THREADS */
}

int
main (int, ACE_TCHAR *[])
{
  ACE_START_TEST (ACE_TEXT ("Thread_Mutex_Test"));

  spawn ();

  ACE_END_TEST;
  return 0;
}
