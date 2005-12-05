/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  Memory Block handling functions; should be used whenever
//               using memcpy is considered; features
//               - typesafe conversion,
//		         - compile-time memory read-write control, 
//               - convenient windows api interfaces and
//               - also a copy-on-write (COW) implementation
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : perfect
//    formatting             : ok
//    documentation          : poor
//    test coverage          : ok
//    names                  : poor
//    style guide conformance: poor
//    technical soundness    : fair
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : fair
//    dependencies           : fair
//    cheesyness             : fair
//
//    overall review status  : fair
//
//    recommendations:
//       - make names styleguide conform
//       - improve usability and integration with Logger
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_console_functions_h_
#define _included_asl_console_functions_h_

#include "settings.h"

#include <iostream>

namespace asl {;

/*! \addtogroup aslbase */
/* @{ */
    bool isTTY(std::ostream & os);

    std::ostream & TTYGREEN (std::ostream & os );

    std::ostream & TTYRED (std::ostream & os );
    std::ostream & TTYYELLOW (std::ostream & os );
    std::ostream & TTYBLUE (std::ostream & os );

    std::ostream & ENDCOLOR (std::ostream & os );
/* @} */
}

#endif
