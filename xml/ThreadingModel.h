/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: ThreadingModel.h,v $
//
//   $Revision: 1.2 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _asl_xml_ThreadingModel_included_
#define _asl_xml_ThreadingModel_included_

#include <asl/AtomicCount.h>


namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    typedef asl::SingleThreaded ThreadingModel;
    
    /* @} */
} //Namespace dom

#endif
