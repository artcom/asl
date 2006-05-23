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
// Description: command line argument parser
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : fair
//    formatting             : ok
//    documentation          : poor
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : fair
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendation: 
//       - improve documentation
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_assure_h_
#define _ac_assure_h_

#include "Exception.h"
#include "Logger.h"

// Assure is intentionally not in the asl namespace to improve readability
// when it is beeing used

/*! \addtogroup aslbase */
/* @{ */

namespace AssurePolicy {
    
    DEFINE_NESTED_EXCEPTION(AssurePolicy, Violation, asl::Exception)

#ifdef check
	#ifndef _SETTING_NO_UNDEF_WARNING_ 
		#warning Symbol 'check' defined as macro, undefining. (Outrageous namespace pollution by Apple's AssertMacros.h, revealing arrogance and incompetence)
	#endif
	#undef check
#endif

    struct Throw {
            static void check( bool myExpressionResult, const char * myExpression, 
                    const char * mySourceFileName, unsigned long mySourceLine)
            {
                if (!myExpressionResult) {
                    throw Violation(myExpression, asl::location_string(mySourceFileName,mySourceLine));
                }
            }

    };

    struct Ignore {
        static void check( bool , const char *, const char *, unsigned long) {}
    };

    struct Warn {
        static void check( bool myExpressionResult, const char * myExpression, 
                const char * mySourceFileName, unsigned long mySourceLine)
        {
            if (!myExpressionResult) {
                AC_WARNING << "AssurePolicy::Warn:check failed: (" << myExpression << ") in " 
                    << asl::location_string(mySourceFileName, mySourceLine) << std::endl;
            }
        }

    };

    struct Exit {
        static void check( bool myExpressionResult, const char * myExpression, 
                const char * mySourceFileName, unsigned long mySourceLine)
        {
            if (!myExpressionResult) {
                AC_ERROR << "AssurePolicy::Exit:check failed: (" << myExpression << ") in " 
                    << asl::location_string(mySourceFileName, mySourceLine) << std::endl;
                std::cerr << "Exiting with return code -1." << std::endl;
                exit(-1);
            }
        }

    };

    struct Abort {
        static void check( bool myExpressionResult, const char * myExpression, 
                const char * mySourceFileName, unsigned long mySourceLine)
        {
            if (!myExpressionResult) {
                AC_FATAL << "AssurePolicy::Exit:check failed: (" << myExpression << ") in " 
                    << asl::location_string(mySourceFileName, mySourceLine) << std::endl;
                AC_FATAL << "Aborting immediately." << std::endl;
                abort();
            }
        }

    };
}

template <class Policy>
struct Assure {
    
//   static void check(bool myExpressionResult,
    Assure(bool myExpressionResult,
           const char * myExpression, 
           const char * mySourceFileName,
           unsigned long mySourceLine)
    {
        Policy::check(myExpressionResult, myExpression, mySourceFileName, mySourceLine);
    }
};

#ifndef ASSURE_POLICY
    #define ASSURE_POLICY AssurePolicy::Warn
#endif

// you should rarely need to use this one directly, but you can if you need an explicit policy
// and a custom message:
//#define ASSURE_MSG_WITH(POLICY,EXP,MSG) Assure<POLICY>::check(EXP, MSG, __FILE__, __LINE__);
#define ASSURE_MSG_WITH(POLICY,EXP,MSG) { Assure<POLICY> myFooBar(EXP, MSG, __FILE__, __LINE__); }

// use this if you want to show a custom message instead of the expression
// and the policy define in ASSURED_POLICY
#define ASSURE_MSG(EXP, MSG) ASSURE_MSG_WITH(ASSURE_POLICY,EXP, MSG);

// use this one if you want to use it with a explicit policy:
#define ASSURE_WITH(POLICY,EXP) ASSURE_MSG_WITH(POLICY,EXP, #EXP);

// this is the normal one using the ASSURE_POLICY preprozessor definition
#define ASSURE(EXP) ASSURE_WITH(ASSURE_POLICY,EXP);

/* @} */

#endif

