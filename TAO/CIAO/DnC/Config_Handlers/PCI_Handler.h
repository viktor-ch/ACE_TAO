//==================================================================
/**
 *  @file  PCI_Handler.h
 *
 *  $Id$
 *
 *  @author George Edwards <g.edwards@vanderbilt.edu>
 */
//=====================================================================

#ifndef PCI_HANDLER_H
#define PCI_HANDLER_H
#include /**/ "ace/pre.h"

#include "DeploymentC.h"
#include "Config_Handler_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/SString.h"
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include "XercesString.h"

using Config_Handler::XStr;
using xercesc::XMLUni;
using xercesc::XMLString;
using xercesc::XMLException;
using xercesc::DOMException;
using xercesc::DOMBuilder;
using xercesc::DOMImplementationRegistry;
using xercesc::DOMImplementationLS;
using xercesc::DOMImplementation;
using xercesc::DOMText;
using xercesc::DOMNamedNodeMap;
using xercesc::DOMLocator;
using xercesc::DOMError;
using xercesc::DOMDocument;
using xercesc::DOMDocumentTraversal;
using xercesc::DOMNodeIterator;
using xercesc::DOMNode;
using xercesc::DOMNodeFilter;

namespace CIAO 
{
  namespace Config_Handler
  {

    /**
     * @class PCI_Handler
     *
     * @brief Handler class for <PackagedComponentImplementation> type 
     *
     * This class is within the Component Data Model subpackage of the
     * Deployment & Configuration package.
     *
     * This class defines handler methods to parse the aforementioned type
     * in the description files. The corresponding CORBA IDL type for this
     * element is returned.
     */

    class PCI_Handler
    {
    public:

      /// constructor
      PCI_Handler (DOMDocument* doc, unsigned long filter_);

      /// constructor
      PCI_Handler (DOMNodeIterator* iter, bool release = false);

      /// destructor
      ~PCI_Handler();

      /// Process the package configuration
      void process_PackagedComponentImplementation (::Deployment::PackagedComponentImplementation &pci);

      /// Process the label attribute
      void process_name (const XMLCh* name, ::Deployment::PackagedComponentImplementation &pci);

    private:

      DOMDocument* doc_;

      DOMNode* root_;

      unsigned long filter_;

      DOMNodeIterator* iter_;

      bool release_;

    };

  }

}

#include /**/ "ace/post.h"

#endif /* PCI_HANDLER_H */
