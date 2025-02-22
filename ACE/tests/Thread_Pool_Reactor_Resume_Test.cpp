
//=============================================================================
/**
 *  @file    Thread_Pool_Reactor_Resume_Test.cpp
 *
 *    This program is an additional torture test of thread pool
 *    reactors.  This test  is based on Thread_Pool_Reactor_Test.cpp
 *    in $ACE_ROOT/tests. This test differs from the other one
 *    slightly. The TP reactor is instantiated with the
 *    with a value of 1 for the <resume_flag> argument. The server
 *    threads during the  handle_input call resumes the handle that
 *    would have been suspended by the reactor.
 *
 *    Usage: Thread_Pool_Reactor_Test [-r <hostname:port#>]
 *              [-s <server thr#>] [-c <client thr#>] [-d <delay>]
 *              [-i <client conn attempt#>] [-n <client request# per conn>]
 *
 *    Default value:
 *        <hostname:port#>:       ACE_DEFAULT_RENDEZVOUS
 *        <server thr#>:          ACE_MAX_THREADS
 *        <client thr#>:          ACE_MAX_ITERATIONS
 *        <client conn attempt#>: ACE_MAX_ITERATIONS
 *        <client req# per conn>: ACE_MAX_THREADS
 *        <delay>:                50 usec
 *
 *  @author Balachandran Natarajan <bala@cs.wustl.edu>
 */
//=============================================================================


#include "test_config.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Get_Opt.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Acceptor.h"
#include "ace/Thread_Manager.h"
#include "ace/TP_Reactor.h"


#if defined (ACE_HAS_THREADS) && !defined ACE_LACKS_ACCEPT

#include "Thread_Pool_Reactor_Resume_Test.h"
using ACCEPTOR = ACE_Strategy_Acceptor<Request_Handler, ACE_SOCK_Acceptor>;

// Accepting end point.  This is actually "localhost:10010", but some
// platform couldn't resolve the name so we use the IP address
// directly here.
static const ACE_TCHAR *rendezvous = ACE_TEXT ("127.0.0.1:10010");

// Total number of server threads.
static size_t svr_thrno = ACE_MAX_THREADS;


// Default network parameters (MAX_BINDS and system buffers) are too small
// for full test on some platforms; add platforms that can't handle too many
// connection simultaneously here.
#if defined (ACE_VXWORKS) || defined (ACE_HAS_PHARLAP)
#define ACE_LOAD_FACTOR /2
#else
#define ACE_LOAD_FACTOR
#endif

// Total number of client threads.
static size_t cli_thrno = ACE_MAX_THREADS ACE_LOAD_FACTOR;

// Total connection attempts of a client thread.
static size_t cli_conn_no = ACE_MAX_ITERATIONS ACE_LOAD_FACTOR;

// Total requests a client thread sends.
static size_t cli_req_no = ACE_MAX_THREADS ACE_LOAD_FACTOR;

// Delay before a thread sending the next request (in msec.)
static int req_delay = 50;

static void
parse_arg (int argc, ACE_TCHAR *argv[])
{
  //FUZZ: disable check_for_lack_ACE_OS
  ACE_Get_Opt getopt (argc, argv, ACE_TEXT ("r:s:c:d:i:n:"));

  int c;

  while ((c = getopt ()) != -1)
    {
  //FUZZ: enable check_for_lack_ACE_OS
      switch (c)
        {
        case 'r':                   // hostname:port
          rendezvous = getopt.opt_arg ();
          break;
        case 's':
          svr_thrno = ACE_OS::atoi (getopt.opt_arg ());
          break;
        case 'c':
          cli_thrno = ACE_OS::atoi (getopt.opt_arg ());
          break;
        case 'd':
          req_delay = ACE_OS::atoi (getopt.opt_arg ());
          break;
        case 'i':
          cli_conn_no = ACE_OS::atoi (getopt.opt_arg ());
          break;
        case 'n':
          cli_req_no = ACE_OS::atoi (getopt.opt_arg ());
          break;
        default:
          ACE_ERROR ((LM_ERROR,
                      "Usage: Thread_Pool_Reactor_Resume_Test [-r <hostname:port#>]"
                      "\t[-s <server thr#>] [-c <client thr#>] [-d <delay>]"
                      "\t[-i <client conn attempt#>]"
                      "[-n <client request# per conn>]\n"));
          break;
        }
    }
}

Request_Handler::Request_Handler (ACE_Thread_Manager *thr_mgr)
  : ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> (thr_mgr),
    nr_msgs_rcvd_(0)
{
  // Enable reference counting.
  this->reference_counting_policy ().value
    (ACE_Event_Handler::Reference_Counting_Policy::ENABLED);

  // Make sure we use TP_Reactor with this class (that's the whole
  // point, right?)
  this->reactor (ACE_Reactor::instance ());
}

int
Request_Handler::open (void *arg)
{
  // Open base class.
  int result =
    ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>::open (arg);

  // Return on error.
  if (result == -1)
    return -1;

  // Else we have successfully registered with the Reactor.  Give our
  // ownership to the Reactor.
  this->remove_reference ();

  // Return result.
  return result;
}

Request_Handler::~Request_Handler ()
{
}

int
Request_Handler::resume_handler ()
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%t) resume_handler () called\n")));
  return 1;
}

int
Request_Handler::handle_input (ACE_HANDLE fd)
{
  ACE_TCHAR buffer[BUFSIZ];
  ACE_TCHAR len = 0;
  ssize_t result = this->peer ().recv (&len, sizeof (ACE_TCHAR));

  if (result > 0
      && this->peer ().recv_n (buffer, len * sizeof (ACE_TCHAR))
          == static_cast<ssize_t> (len * sizeof (ACE_TCHAR)))
    {
      ++this->nr_msgs_rcvd_;

      // Now the handle_input method has done what it can do, namely
      // read the data from the socket we can just resume the handler
      // at this point
      ACE_DEBUG ((LM_DEBUG,
                  "(%t) svr input; fd: 0x%x; input: %s\n",
                  fd,
                  buffer));
      if (ACE_OS::strcmp (buffer, ACE_TEXT ("shutdown")) == 0)
          ACE_Reactor::instance()->end_reactor_event_loop ();

      this->reactor ()->resume_handler (fd);
      return 0;
    }
  else
    {
      ACE_DEBUG ((LM_DEBUG,
                  "(%t) Errno is %d and result is %d\n",
                  ACE_ERRNO_GET, result));
      ACE_DEBUG ((LM_DEBUG,
                  "(%t) Request_Handler: 0x%x peer closed (0x%x)\n",
                  this, fd));
    }
  return -1;
}

int
Request_Handler::handle_close (ACE_HANDLE fd, ACE_Reactor_Mask)
{
  ACE_DEBUG ((LM_DEBUG,
              "(%t) svr close; fd: 0x%x, rcvd %d msgs\n",
              fd,
              this->nr_msgs_rcvd_));
  if (this->nr_msgs_rcvd_ != cli_req_no)
    ACE_ERROR((LM_ERROR,
               ACE_TEXT ("(%t) Handler 0x%x: Expected %d messages; got %d\n"),
               this,
               cli_req_no,
               this->nr_msgs_rcvd_));

  return 0;
}

static int
reactor_event_hook (ACE_Reactor *)
{
  ACE_DEBUG ((LM_DEBUG,
              "(%t) handling events ....\n"));

  return 0;
}

static ACE_THR_FUNC_RETURN
svr_worker (void *)
{
  // Server thread function.
  int result =
    ACE_Reactor::instance ()->run_reactor_event_loop (&reactor_event_hook);

  if (result == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("(%t) %p\n"),
                       ACE_TEXT ("Error handling events")),
                      0);

  ACE_DEBUG ((LM_DEBUG,
              "(%t) I am done handling events. Bye, bye\n"));

  return 0;
}

static ACE_THR_FUNC_RETURN
cli_worker (void *arg)
{
  // Client thread function.
  ACE_INET_Addr addr (rendezvous);
  ACE_SOCK_Stream stream;
  ACE_SOCK_Connector connect;
  ACE_Time_Value delay (0, req_delay);
  size_t len = * reinterpret_cast<ACE_TCHAR *> (arg);

  for (size_t i = 0 ; i < cli_conn_no; i++)
    {
      if (connect.connect (stream, addr) < 0)
        {
          ACE_ERROR ((LM_ERROR,
                      ACE_TEXT ("(%t) %p\n"),
                      ACE_TEXT ("connect")));
          continue;
        }

      for (size_t j = 0; j < cli_req_no; j++)
        {
          ACE_DEBUG ((LM_DEBUG,
                      "(%t) conn_worker handle = %x, req = %d\n",
                      stream.get_handle (),
                      j+1));
          if (stream.send_n (arg,
                             (len + 1) * sizeof (ACE_TCHAR)) == -1)
            {
              ACE_ERROR ((LM_ERROR,
                          ACE_TEXT ("(%t) %p\n"),
                          ACE_TEXT ("send_n")));
              continue;
            }
          ACE_OS::sleep (delay);
        }

      stream.close ();
    }

  return 0;
}

static ACE_THR_FUNC_RETURN
worker (void *)
{
  ACE_OS::sleep (3);
  const ACE_TCHAR *msg = ACE_TEXT ("Message from Connection worker");
  ACE_TCHAR buf [BUFSIZ];
  buf[0] = static_cast<ACE_TCHAR> ((ACE_OS::strlen (msg) + 1));
  ACE_OS::strcpy (&buf[1], msg);

  ACE_INET_Addr addr (rendezvous);

  ACE_DEBUG((LM_DEBUG,
             "(%t) Spawning %d client threads...\n",
             cli_thrno));
  int grp = ACE_Thread_Manager::instance ()->spawn_n (cli_thrno,
                                                      &cli_worker,
                                                      buf);
  ACE_TEST_ASSERT (grp != -1);

  ACE_Thread_Manager::instance ()->wait_grp (grp);

  ACE_DEBUG ((LM_DEBUG,
              "(%t) Client threads done; shutting down...\n"));
  ACE_SOCK_Stream stream;
  ACE_SOCK_Connector connect;

  if (connect.connect (stream, addr) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("(%t) %p Error while connecting\n"),
                ACE_TEXT ("connect")));

  const ACE_TCHAR *sbuf = ACE_TEXT ("\011shutdown");

  ACE_DEBUG ((LM_DEBUG,
              "shutdown stream handle = %x\n",
              stream.get_handle ()));

  if (stream.send_n (sbuf, (ACE_OS::strlen (sbuf) + 1) * sizeof (ACE_TCHAR)) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("(%t) %p\n"),
                ACE_TEXT ("send_n")));

  ACE_DEBUG ((LM_DEBUG,
              "Sent message of length  = %d\n",
              ACE_OS::strlen (sbuf)));
  stream.close ();

  return 0;
}

int
run_main (int argc, ACE_TCHAR *argv[])
{
  ACE_START_TEST (ACE_TEXT ("Thread_Pool_Reactor_Resume_Test"));
  parse_arg (argc, argv);

  // Changed the default
  ACE_TP_Reactor sr;


  ACE_Reactor new_reactor (&sr);
  ACE_Reactor::instance (&new_reactor);

  ACCEPTOR acceptor;
  ACE_INET_Addr accept_addr (rendezvous);

  if (acceptor.open (accept_addr) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%p\n"),
                       ACE_TEXT ("open")),
                      1);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT ("(%t) Spawning %d server threads...\n"),
             svr_thrno));
  ACE_Thread_Manager::instance ()->spawn_n (svr_thrno,
                                            svr_worker);
  ACE_Thread_Manager::instance ()->spawn (worker);

  ACE_Thread_Manager::instance ()->wait ();

  ACE_END_TEST;
  return 0;
}

#else
int
run_main (int, ACE_TCHAR *[])
{
  ACE_START_TEST (ACE_TEXT ("Thread_Pool_Reactor_Resume_Test"));

  ACE_ERROR ((LM_INFO,
              "threads/accept not supported on this platform\n"));

  ACE_END_TEST;
  return 0;
}
#endif /* ACE_HAS_THREADS */
