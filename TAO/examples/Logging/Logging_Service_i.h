

//=============================================================================
/**
 *  @file    Logging_Service_i.h
 *
 *    This class implements a simple "logger" CORBA server for the Logging
 *    service using skeletons generated by the TAO ORB IDL compiler.
 *
 *  @author Nagarajan Surendran <naga@cs.wustl.edu> Matthew Braun <mjb2@cec.wustl.edu>
 */
//=============================================================================


#ifndef _LOG_SERVER_H
#define _LOG_SERVER_H

#include "ace/Get_Opt.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Log_Msg.h"
#include "tao/Utils/ORB_Manager.h"
#include "orbsvcs/CosNamingC.h"
#include "orbsvcs/Naming/Naming_Client.h"
#include "Logger_i.h"
#include "LoggerS.h"
#include "LoggerC.h"

/**
 * @class Logger_Server
 *
 * @brief Defines a Logger_Server class that implements the functionality
 * of a server process as an object.
 *
 * The interface is quite simple. A server program has to call
 * init to initialize the logger_server's state and then call run
 * to run the orb.
 */
class Logger_Server
{
public:
  // = Initialization and termination methods.
  /// Constructor.
  Logger_Server (void);

  /// Destructor.
  ~Logger_Server (void);

  /// Initialize the Logger_Server state - parsing arguments and ...
  int init (int argc,
            ACE_TCHAR **argv);

  /// Run the ORB.
  int run (void);

  /// This function creates and returns a logger with the given <name>.
  /// Currently, <name> is unused.
  Logger_ptr make_logger (const char *name);

private:
  /// Parses the commandline arguments.
  int parse_args (void);

  /// Initialises the name server and registers logger_factory with the
  /// name server.
  int init_naming_service (void);

  /// The ORB manager.
  TAO_ORB_Manager orb_manager_;

  /// helper class for getting access to Naming Service.
  TAO_Naming_Client my_name_server_;

  /// Implementation object of the Logger_Factory.
  Logger_Factory_i factory_impl_;

  /// Factory_var to register with NamingService.
  Logger_Factory_var factory_;

  /// Number of commandline arguments.
  int argc_;

  /// commandline arguments.
  ACE_TCHAR **argv_;

  /// The id to give the Logger_Factory instance (defaults to
  /// "LoggingService")
  const ACE_TCHAR* service_name_;
};

#endif /* _LOG_SERVER_H */
