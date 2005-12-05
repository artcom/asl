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
// Description:  Exception Base Class & Macros
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : perfect
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
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendations:
//       - make a documentation
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_ExceptionBase_
#define _included_asl_ExceptionBase_

#include <iostream>
#include <string>

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    class Exception {
    public:

        Exception(const std::string & what = "", const std::string & where ="")
            : _what(what), _where(where), _name("Exception") {};

        virtual ~Exception() {}

        Exception(const Exception& ex)
            : _what(ex.what()), _where(ex.where()), _name(ex.name()) {}

        Exception& operator=(const Exception& ex) {
            set(ex.what(),ex.where(),ex.name());
            return *this;
        }
        virtual const std::string & what() const {
            return _what;
        }
        virtual const std::string & where() const {
            return _where;
        }
        virtual const char * name() const {
            return _name;
        }
        void appendWhat(const std::string & whatelse) {
            _what += "\n"+whatelse;
        }
    protected:
        virtual void set(const std::string & what,
                         const std::string & where,
                         const char * name) const {
            _what = what;
            _where = where;
            _name = name;
        }
        Exception(const std::string & what, const std::string & where, const char * name)
            : _what(what), _where(where), _name(name) {};

    private:
        mutable std::string _what;
        mutable std::string _where;
        mutable const char * _name;
    };

	std::string file_string(const char* file_name);
    std::string line_string(unsigned line_number);
    std::string location_string(const char* file_name, unsigned line_number);

    std::string compose_message(const Exception & ex);

    inline
    std::ostream& operator<<(std::ostream& os,const Exception & ex) {
        return os << compose_message(ex) << std::endl;
    }

    #define PLUS_FILE_LINE asl::location_string(__FILE__,__LINE__)
    #define JUST_FILE_LINE asl::line_string(__LINE__),asl::file_string(__FILE__)

    #define DEFINE_NAMED_EXCEPTION(CLASS,NAME,BASECLASS)\
    class CLASS : public BASECLASS {\
        public:\
               CLASS(const std::string & what, const std::string & where)\
                   : BASECLASS(what,where, # NAME ) {}\
        protected:\
                  CLASS(const std::string & what, const std::string & where, const char * name)\
                      : BASECLASS(what,where, name) {}\
    };

    #define DEFINE_EXCEPTION(CLASS,BASECLASS) DEFINE_NAMED_EXCEPTION(CLASS,CLASS,BASECLASS)
    #define DEFINE_NESTED_EXCEPTION(INSIDECLASS,CLASS,BASECLASS) DEFINE_NAMED_EXCEPTION(CLASS,INSIDECLASS::CLASS,BASECLASS)

    // Some generic exceptions
    DEFINE_EXCEPTION(ParseException, Exception);
    DEFINE_EXCEPTION(NotYetImplemented, Exception)
    DEFINE_EXCEPTION(InputOutputFailure, Exception)

    /* @} */

    
} //Namespace asl



#endif
