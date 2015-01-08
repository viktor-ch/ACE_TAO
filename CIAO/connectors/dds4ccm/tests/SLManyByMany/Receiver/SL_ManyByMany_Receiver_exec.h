// -*- C++ -*-
#ifndef CIAO_SL_MANYBYMANY_RECEIVER_EXEC_4U4QJF_H_
#define CIAO_SL_MANYBYMANY_RECEIVER_EXEC_4U4QJF_H_

#include /**/ "ace/pre.h"

#include "SL_ManyByMany_ReceiverEC.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include /**/ "Receiver_exec_export.h"
#include "tao/LocalObject.h"

namespace CIAO_SL_ManyByMany_Receiver_Impl
{
  typedef ACE_Atomic_Op <TAO_SYNCH_MUTEX, CORBA::ULong > Atomic_Long;
  typedef ACE_Atomic_Op <TAO_SYNCH_MUTEX, CORBA::Boolean > Atomic_Bool;

  class Receiver_exec_i;

  /**
   * Read action generator
   */

  class read_action_Generator
    : public ACE_Event_Handler
  {
  public:
    read_action_Generator (Receiver_exec_i &callback);

    ~read_action_Generator ();

    /// Handle the timeout.
    virtual int handle_timeout (const ACE_Time_Value &tv,
                                const void *arg);

  private:
    /// Maintains a handle that actually process the event
    Receiver_exec_i &pulse_callback_;

  };

  /**
   * Provider Executor Implementation Class: info_out_data_listener_exec_i
   */

  class info_out_data_listener_exec_i
    : public virtual ::SL_ManyByMany::SLManyByManyConnector::CCM_StateListener,
      public virtual ::CORBA::LocalObject
  {
  public:
    info_out_data_listener_exec_i (
      ::SL_ManyByMany::CCM_Receiver_Context_ptr ctx,
      Atomic_Long &no_operation,
      Atomic_Long &on_creation,
      Atomic_Long &on_many_update,
      Atomic_Long &on_many_upd_trigger,
      Atomic_Long &on_deletion,
      Atomic_Bool &create_data,
      Atomic_Bool &update_data,
      Atomic_Long &samples_read);
    virtual ~info_out_data_listener_exec_i (void);

    //@{
    /** Operations and attributes from SL_ManyByMany::SLManyByManyConnector::StateListener. */

    virtual
    void on_creation (const ::TestTopic & datum,
    const ::CCM_DDS::ReadInfo & info);

    virtual
    void on_one_update (const ::TestTopic & datum,
    const ::CCM_DDS::ReadInfo & info);

    virtual
    void on_many_updates (const ::TestTopicSeq & data,
    const ::CCM_DDS::ReadInfoSeq & infos);

    virtual
    void on_deletion (const ::TestTopic & datum,
    const ::CCM_DDS::ReadInfo & info);
    //@}

  private:
    ::SL_ManyByMany::CCM_Receiver_Context_var ciao_context_;
     Atomic_Long &no_operation_;
     Atomic_Long &on_creation_;
     Atomic_Long &on_many_update_;
     Atomic_Long &on_many_upd_trigger_;
     Atomic_Long &on_deletion_;
     Atomic_Bool &create_data_;
     Atomic_Bool &update_data_;
     Atomic_Long &samples_read_;
  };

  /**
   * Provider Executor Implementation Class: info_out_status_exec_i
   */

  class info_out_status_exec_i
    : public virtual ::CCM_DDS::CCM_PortStatusListener,
      public virtual ::CORBA::LocalObject
  {
  public:
    info_out_status_exec_i (
      ::SL_ManyByMany::CCM_Receiver_Context_ptr ctx);
    virtual ~info_out_status_exec_i (void);

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
    ::SL_ManyByMany::CCM_Receiver_Context_var ciao_context_;
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

    virtual ::SL_ManyByMany::SLManyByManyConnector::CCM_StateListener_ptr
    get_info_out_data_listener (void);

    virtual ::CCM_DDS::CCM_PortStatusListener_ptr
    get_info_out_status (void);
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
    void read_all(void);

    //@}

  private:
    ::SL_ManyByMany::CCM_Receiver_Context_var ciao_context_;

    //@{
    /** Component attributes. */
    ::SL_ManyByMany::SLManyByManyConnector::CCM_StateListener_var ciao_info_out_data_listener_;
    ::CCM_DDS::CCM_PortStatusListener_var ciao_info_out_status_;
    //@}

    //@{
    /** User defined members. */
    read_action_Generator * ticker_;

    Atomic_Long no_operation_;
    Atomic_Long on_creation_;
    Atomic_Long on_many_update_;
    Atomic_Long on_many_upd_trigger_;
    Atomic_Long on_deletion_;
    Atomic_Bool create_data_;
    Atomic_Bool update_data_;
    Atomic_Long reader_data_;
    Atomic_Long samples_read_;
    //@}

    //@{
    /** User defined private operations. */

    //@}

    /// Get the ACE_Reactor
    ACE_Reactor* reactor (void);
  };

  extern "C" RECEIVER_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_SL_ManyByMany_Receiver_Impl (void);
}

#include /**/ "ace/post.h"

#endif /* ifndef */
