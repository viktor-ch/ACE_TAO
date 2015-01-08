// -*- C++ -*-
/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v1.8.3
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 *       http://www.cs.wustl.edu/~schmidt/doc-center.html
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       http://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     http://www.dre.vanderbilt.edu/~schmidt/TAO.html
 **/
#ifndef CIAO_CFTLE_TEST_RECEIVER_EXEC_NV4QAC_H_
#define CIAO_CFTLE_TEST_RECEIVER_EXEC_NV4QAC_H_

#include /**/ "ace/pre.h"

#include "CFTLE_Test_ReceiverEC.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include /**/ "Receiver_exec_export.h"
#include "tao/LocalObject.h"

namespace CIAO_CFTLE_Test_Receiver_Impl
{
  typedef ACE_Atomic_Op <TAO_SYNCH_MUTEX, CORBA::Long> Atomic_Long;

  /**
   * Provider Executor Implementation Class: info_listen_data_listener_exec_i
   */

  class info_listen_data_listener_exec_i
    : public virtual ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::CCM_Listener,
      public virtual ::CORBA::LocalObject
  {
  public:
    info_listen_data_listener_exec_i (
      ::CFTLE_Test::CCM_Receiver_Context_ptr ctx,
      ACE_Thread_ID &thread_id,
      Atomic_Long &samples_received);
    virtual ~info_listen_data_listener_exec_i (void);

    //@{
    /** Operations and attributes from CFTLE_Test::ContentFilteredTopicListenEventTestConnector::Listener. */

    virtual
    void on_one_data (const ::ContentFilteredTopicListenEventTest & datum,
    const ::CCM_DDS::ReadInfo & info);

    virtual
    void on_many_data (const ::ContentFilteredTopicListenEventTestSeq & data,
    const ::CCM_DDS::ReadInfoSeq & infos);
    //@}

  private:
    ::CFTLE_Test::CCM_Receiver_Context_var ciao_context_;
    ACE_Thread_ID &thread_id_;
    Atomic_Long &samples_received_;
  };

  /**
   * Provider Executor Implementation Class: info_listen_status_exec_i
   */

  class info_listen_status_exec_i
    : public virtual ::CCM_DDS::CCM_PortStatusListener,
      public virtual ::CORBA::LocalObject
  {
  public:
    info_listen_status_exec_i (
      ::CFTLE_Test::CCM_Receiver_Context_ptr ctx);
    virtual ~info_listen_status_exec_i (void);

    //@{
    /** Operations and attributes from CCM_DDS::PortStatusListener. */

    virtual
    void on_requested_deadline_missed (::DDS::DataReader_ptr the_reader,
    const ::DDS::RequestedDeadlineMissedStatus & status);

    virtual
    void on_sample_lost (::DDS::DataReader_ptr the_reader,
    const ::DDS::SampleLostStatus & status);
    //@}

  private:
    ::CFTLE_Test::CCM_Receiver_Context_var ciao_context_;
  };

  /**
   * Component Executor Implementation Class: Receiver_exec_i
   */

  class Receiver_exec_i
    : public virtual Receiver_Exec,
      public virtual ::CORBA::LocalObject
  {
  public:
    Receiver_exec_i (void);
    virtual ~Receiver_exec_i (void);

    //@{
    /** Supported operations and attributes. */

    //@}

    //@{
    /** Component attributes and port operations. */

    virtual ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::CCM_Listener_ptr
    get_info_listen_data_listener (void);

    virtual ::CCM_DDS::CCM_PortStatusListener_ptr
    get_info_listen_status (void);

    virtual ::CORBA::UShort keys (void);

    virtual void keys (::CORBA::UShort keys);

    virtual ::CORBA::UShort iterations (void);

    virtual void iterations (::CORBA::UShort iterations);
    //@}

    //@{
    /** Operations from Components::SessionComponent. */
    virtual void set_session_context (::Components::SessionContext_ptr ctx);
    virtual void configuration_complete (void);
    virtual void ccm_activate (void);
    virtual void ccm_passivate (void);
    virtual void ccm_remove (void);
    //@}

    //@{
    /** User defined public operations. */
    void start (void);

    //@}

  private:
    ::CFTLE_Test::CCM_Receiver_Context_var ciao_context_;

    //@{
    /** Component attributes. */
    ::CFTLE_Test::ContentFilteredTopicListenEventTestConnector::CCM_Listener_var ciao_info_listen_data_listener_;
    ::CCM_DDS::CCM_PortStatusListener_var ciao_info_listen_status_;

    ::CORBA::UShort keys_;

    ::CORBA::UShort iterations_;
    //@}

    //@{
    /** User defined members. */
    ACE_Thread_ID thread_id_listener_;

    CORBA::Long samples_expected_;
    Atomic_Long samples_received_;

    //@}

    //@{
    /** User defined private operations. */

    //@}
  };

  extern "C" RECEIVER_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_CFTLE_Test_Receiver_Impl (void);
}

#include /**/ "ace/post.h"

#endif /* ifndef */
