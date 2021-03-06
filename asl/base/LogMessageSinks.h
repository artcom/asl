/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2012, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  A small collection of log message sinks
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _ac_asl_LogMessageSinks_h_
#define _ac_asl_LogMessageSinks_h_

#include "asl_base_settings.h"

#include <cstdlib>

#include "Logger.h"
#include "console_functions.h"

namespace asl {

    class FarewellMessageSink : public MessageSink {
    public:
        FarewellMessageSink() {}
        void push(Severity theSeverity, const std::string & theMessage) {
            if (theSeverity == SEV_FATAL) {
                abort();
            }
        };
    private:
    };

    class StreamPrinter : public MessageSink {
    public:
        StreamPrinter(std::ostream & theStream) : _myStream(theStream) {}
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();

            switch (theSeverity) {
                case SEV_FATAL :
                case SEV_ERROR :
                    _myStream << TTYRED;
                    break;
                case SEV_WARNING:
                    _myStream << TTYYELLOW;
                    break;
                default:
                    // normal black, do nothing
                    break;
            }
            _myStream << theMessage << ENDCOLOR << std::endl;
            myLock.unlock();
        };
    private:
        static ThreadLock myLock;
        std::ostream & _myStream;
    };

    class ASL_BASE_DECL FilePrinter : public MessageSink {
    public:
        FilePrinter(const std::string & theFilename) : _myStream(theFilename.c_str()) {}
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();
            _myStream << theMessage << std::endl;
            myLock.unlock();
        };
    private:
        static ThreadLock myLock;
        std::ofstream _myStream;
    };

#ifdef _WIN32
#include "windows.h"
    class OutputWindowPrinter : public MessageSink {
    public:
        OutputWindowPrinter() {}
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();
            OutputDebugString((theMessage + "\n").c_str());
            myLock.unlock();
        }
    private:
        static ThreadLock myLock;
    };
#endif

#ifndef _WIN32
#include <syslog.h>
    class SyslogPrinter : public MessageSink {
    public:
        SyslogPrinter() {
            openlog("Y60", LOG_CONS, LOG_LOCAL6);
        }
        void push(Severity theSeverity, const std::string & theMessage) {
            myLock.lock();

            int syslog_sev = -1;
            switch (theSeverity) {
                case SEV_FATAL :
                    syslog_sev = LOG_EMERG;
                    break;
                case SEV_ERROR :
                    syslog_sev = LOG_ERR;
                    break;
                case SEV_WARNING:
                    syslog_sev = LOG_WARNING;
                    break;
                case SEV_INFO:
                    syslog_sev = LOG_INFO;
                    break;
                case SEV_DEBUG:
                    syslog_sev = LOG_DEBUG;
                    break;
                case SEV_PRINT:
                    syslog_sev = LOG_NOTICE;
                    break;
                default:
                    syslog_sev = -1;
                    break;
            }
            
            syslog(syslog_sev, "%s", (theMessage).c_str());
            myLock.unlock();
        }
        ~SyslogPrinter() {
            closelog();
        }
    private:
        static ThreadLock myLock;
    };
#endif
}

    /* @} */
#endif

