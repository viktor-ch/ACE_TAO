
#include "orbsvcs/Log_Macros.h"
#include "orbsvcs/Naming/FaultTolerant/FT_Storable_Naming_Context.h"
#include "orbsvcs/Naming/FaultTolerant/FT_Storable_Naming_Context_Factory.h"
#include "orbsvcs/Naming/FaultTolerant/FT_Naming_Manager.h"
#include "orbsvcs/Naming/FaultTolerant/FT_Naming_Replication_Manager.h"

#include "orbsvcs/PortableGroup/PG_Utils.h"
#include "orbsvcs/PortableGroup/PG_Property_Utils.h"

#include "orbsvcs/Naming/Persistent_Context_Index.h"
#include "ace/OS_NS_stdio.h"

#include "ace/Auto_Ptr.h"

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Initialize the static naming manager
TAO_FT_Naming_Manager *TAO_FT_Storable_Naming_Context::naming_manager_ = 0;

TAO_FT_Storable_Naming_Context::TAO_FT_Storable_Naming_Context (CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               const char *poa_id,
                               TAO_Storable_Naming_Context_Factory *cxt_factory,
                               TAO::Storable_Factory *pers_factory)
  : TAO_Storable_Naming_Context (orb,
                                 poa,
                                 poa_id,
                                 cxt_factory,
                                 pers_factory),
    stale_ (false),
    replicator_ (((TAO_FT_Storable_Naming_Context_Factory *)cxt_factory)->replicator())
{
}

CORBA::Boolean
TAO_FT_Storable_Naming_Context::is_object_group (CORBA::Object_ptr obj) const
{
  // Ensure the object is not nil first. If so, it cannot be an ObjectGroup.
  if (CORBA::is_nil (obj))
    return false;

  // If there is a tagged component with tag = IOP::TAG_FT_GROUP in the
  // object reference then it is an object group
  PortableGroup::TagGroupTaggedComponent tagged_component;
  return TAO::PG_Utils::get_tagged_component (obj, tagged_component);
}

CORBA::Object_ptr
TAO_FT_Storable_Naming_Context::resolve (const CosNaming::Name& n)
{
  // Invoke the base class resolve operation to acquire the object at the
  // specified compound name.  Any exceptions should flow back to client.
  CORBA::Object_var resolved_ref = TAO_Storable_Naming_Context::resolve(n);

  try {
    // Make sure object is an object group.
    // We will return the object reference as is all the way back
    // out to the client if not
    if (!this->is_object_group (resolved_ref.in ()))
      return resolved_ref._retn ();

    // If there is no naming manager, we will fail and report an error.
    if ( this->naming_manager_ == 0)
    {
      ORBSVCS_ERROR ((LM_ERROR,
                  ACE_TEXT ("TAO_FT_Storable_Naming_Context::resolve ")
                  ACE_TEXT ("- No NamingManager defined.\n")));

      throw CORBA::INTERNAL ();
    }

    // The Naming Manager will apply the appropriate strategy to get the
    // next object reference from the object group.
    resolved_ref = this->naming_manager_->next_member (resolved_ref.in ());
  }
  catch (const PortableGroup::ObjectGroupNotFound&)
  {
    // This is apparently not an object group, so we should return the
    // object reference itself
    // No action required
  }
  catch (const CORBA::Exception& ex)
  {
    ex._tao_print_exception (
       ACE_TEXT ("TAO_FT_Storable_Naming_Context::resolve - ")
       ACE_TEXT ("Some unhandled error occurred\n"));
    return CORBA::Object::_nil ();
  }

  return resolved_ref._retn ();
}

void
TAO_FT_Storable_Naming_Context::propagate_update_notification (
                   FT_Naming::ChangeType change_type)
{
  if (this->replicator_)
    this->replicator_->send_context_update (this->context_name_,
                                            change_type);
}

void
TAO_FT_Storable_Naming_Context::set_naming_manager (
  TAO_FT_Naming_Manager *mgr_impl)
{
  naming_manager_ = mgr_impl;
}

void
TAO_FT_Storable_Naming_Context::stale (bool is_stale)
{
  this->stale_ = is_stale;
}


bool
TAO_FT_Storable_Naming_Context::stale ()
{
  return stale_;
}

void
TAO_FT_Storable_Naming_Context::context_written ()
{
  FT_Naming::ChangeType change_type;

  if (this->destroyed_)
    change_type = FT_Naming::DELETED;
  else
    change_type = FT_Naming::UPDATED;

  propagate_update_notification (change_type);
}

bool
TAO_FT_Storable_Naming_Context::is_obsolete (time_t stored_time)
{
  // If data has never been loaded, then the context_ object will
  // be a null pointer.
  return ((this->context_ == 0) ||              // Has not been loaded
          this->stale () ||                     // Explicitly marked stale by peer
          (stored_time > this->last_changed_)); // File has been updated
}

TAO_END_VERSIONED_NAMESPACE_DECL
