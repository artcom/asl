/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: misc operating system & environment helper functions
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifdef _WIN32
  #define _WIN32_WINNT 0x0500
#endif

// own header
#include "os_functions.h"

#include "os_functions.impl"
#include "Exception.h"
#include "string_functions.h"
#include "file_functions.h"
#include "Time.h"
#include "Logger.h"

// cycle counter

#include <string>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
  #include <windows.h>
#endif

#ifdef LINUX
  #include <sys/types.h>
  #include <sys/syscall.h>
#endif

#define DB(x) // x;

using namespace std;

namespace asl {

    template ASL_BASE_DECL bool get_environment_var_as(const std::string & theVariable, double& theValue);
    template ASL_BASE_DECL bool get_environment_var_as(const std::string & theVariable, int& theValue);
    template ASL_BASE_DECL bool get_environment_var_as(const std::string & theVariable,
            unsigned int& theValue);
    template ASL_BASE_DECL bool get_environment_var_as(const std::string & theVariable, string& theValue);
    template ASL_BASE_DECL bool get_environment_var_as(const std::string & theVariable,
            asl::Unsigned16& theValue);
    template ASL_BASE_DECL bool get_environment_var_as(const std::string & theVariable, bool& theValue);

#ifdef _WIN32
    // This is executed once on program start.
    char * reallyGetHostname() {
        HINSTANCE           myDLL_Handle;
        typedef UINT (CALLBACK* MYGETCOMPUTERNAME)   (LPTSTR, LPDWORD);
        typedef UINT (CALLBACK* MYGETCOMPUTERNAMEEX) (COMPUTER_NAME_FORMAT, LPTSTR, LPDWORD);
        MYGETCOMPUTERNAME   myGetComputerName;
        MYGETCOMPUTERNAMEEX myGetComputerNameEx;

        DWORD       length = 4096;
        static char myHostname[4096];
        BOOL        ok = false;

        myDLL_Handle = LoadLibrary("kernel32.dll");
        if (myDLL_Handle != NULL) {
#ifdef UNICODE
            myGetComputerNameEx = (MYGETCOMPUTERNAMEEX)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameExW");
#else
            myGetComputerNameEx = (MYGETCOMPUTERNAMEEX)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameExA");
#endif // UNICODE
            if (!myGetComputerNameEx) {
                // Win9x version of kernel32.dll
                AC_DEBUG << "Win9x version of 'kernel32.dll' detected.";
#ifdef UNICODE
                myGetComputerName = (MYGETCOMPUTERNAME)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameW");
#else
                myGetComputerName = (MYGETCOMPUTERNAME)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameA");
#endif // UNICODE
                if (!myGetComputerName) {
                    throw (asl::Exception(
                            "DLL export 'GetComputerName' was not found in Win9x version of 'kernel32.dll'.",
                            PLUS_FILE_LINE));
                } else {
                    ok = myGetComputerName (myHostname, &length);
                }
            } else {
                // call GetComputerNameEx(...)
                ok = myGetComputerNameEx((COMPUTER_NAME_FORMAT)ComputerNameDnsFullyQualified,
                                          myHostname, &length);
            }
            FreeLibrary(myDLL_Handle);
        }
        if (!ok) {
            throw (asl::Exception("Computer name could not be retrieved.", PLUS_FILE_LINE));
        }
        return myHostname;
    }
#else
    // This is executed once on program start.
    char * reallyGetHostname() {
        const size_t myLength = 4096;
        static char myHostname[myLength];
        int myResult = gethostname(myHostname, myLength);
        if (myResult == -1) {
            throw asl::Exception();
        }
        return myHostname;
    }
#endif

#ifdef OSX
    std::string getAppname() {
        static string myPid = asl::as_string(getpid());
        const size_t myLength = 4096;
        static char myAppnameBuffer[myLength];
        static string myCommand (string("ps -c -o command -p ") + myPid + " | tail -1");
        FILE * myProcess = popen(myCommand.c_str(), "r");
        fread(myAppnameBuffer, myLength, 1, myProcess);
        pclose(myProcess);
        std::string myAppname(myAppnameBuffer);
        return myAppname.substr(0, myAppname.size() - 1);
    }
#endif

    const string & hostname() {
        static string hostname(reallyGetHostname());
        return hostname;
    }

    const string & appname() {
        static string appname;
#ifdef _WIN32
        int numArgs;
        LPWSTR * theCmdLine = CommandLineToArgvW((LPCWSTR)GetCommandLine(), &numArgs);
        appname = (const char *)theCmdLine[0];
        GlobalFree (theCmdLine);
#endif
#ifdef LINUX
        static string pid = asl::as_string(getpid());
        static string procstat = asl::readFile(string("/proc/")+pid+"/stat");
        appname = procstat.substr(procstat.find('(')+1,procstat.find(')')-procstat.find('(')-1);
#endif
#ifdef OSX
        appname = getAppname();
#endif
        return appname;
    }

    const string & hostappid() {
        static string hostappid = hostname() + ":" + appname();
        return hostappid;

    }

    string getSessionId() {
#ifdef _WIN32
        HANDLE token;
        DWORD len;
        BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
        if(result)
        { /* NT */
            GetTokenInformation(token, TokenStatistics, NULL, 0, &len);
            LPBYTE data = new BYTE[len];
            GetTokenInformation(token, TokenStatistics, data, len, &len);
            LUID uid = ((PTOKEN_STATISTICS)data)->AuthenticationId;
            delete [ ] data;
            ostringstream myString;
            myString << hex << uid.HighPart;
            myString << hex << uid.LowPart;
            return myString.str();
        } /* NT */
        else
        { /* 16-bit OS */
            return "16bit";
        } /* 16-bit OS */
#else
        return hostappid();
#endif
    }

    inline
    long microns_from_secs(double secs) {
        return long(secs * 1000000);
    }

    inline
    double secs_from_microns(long microns) {
        return double(microns) / 1000000;
    }

    void precision_sleep(double theDuration){
#ifdef _WIN32
        Sleep((DWORD)theDuration);
#else
        asl::Time startTime;
        double stopTime = double(startTime) + theDuration;
        long remainTime = microns_from_secs(theDuration);
        while ( remainTime > 2000) {
            usleep(1000);
            asl::Time currentTime;
            remainTime = microns_from_secs(stopTime - double(currentTime));
        }
        usleep(remainTime);
#endif
    }


    std::string
    expandEnvironment(const std::string & theString) {
        std::string myExpandedString(theString);
        std::string::size_type myOpenPos;
        std::string::size_type myClosePos = 0;

        while (( myOpenPos = myExpandedString.find("${", myClosePos)) != std::string::npos) {
            myClosePos = myExpandedString.find("}", myOpenPos);
            if (myClosePos == std::string::npos) {
                throw ParseException(std::string("Missing right brace in string '") + theString + "'", "expandEnvironment()");
            }
            std::string myVarName = myExpandedString.substr(myOpenPos+2, myClosePos - myOpenPos - 2);
            for (std::string::size_type i= 0; i < myVarName.size(); ++i) {
                if (!is_alpha(myVarName[i]) && !is_underscore(myVarName[i]) && !is_digit(myVarName[i])) {
                    throw ParseException(std::string("Unexpected character in variable name '") + myVarName + "'", "expandEnvironment()");
                }
            }

            DB(AC_TRACE << "myOpenPos:" << myOpenPos << " myClosePos:" << myClosePos << " myVarName:" << myVarName << endl);
            std::string myValue;
            if (get_environment_var(myVarName, myValue)) {
                DB(AC_TRACE << "     myValue: " << myValue << endl;)
                std::string myReplacement(myValue);
                myExpandedString.replace(myOpenPos, myClosePos-myOpenPos+1, myReplacement);
                myClosePos = myOpenPos + myReplacement.size();
            } else {
                DB(AC_TRACE << "     no value " << endl )
                myExpandedString.erase(myOpenPos, myClosePos-myOpenPos+1);
                myClosePos = myOpenPos;
            }
        }
        DB(AC_TRACE << "      expanded: '" << myExpandedString << "'" << endl;)
        return myExpandedString;
    }

    bool
    get_environment_var(const std::string & theVariable, std::string & theValue) {
         const char * myValue = ::getenv(theVariable.c_str());
         if (myValue) {
             theValue = myValue;
             return true;
         } else {
             return false;
         }
    }

    void
    set_environment_var(const std::string & theVariable, const std::string & theValue) {
#ifdef _WIN32
        _putenv((theVariable + "=" + theValue).c_str());
#else
        setenv(theVariable.c_str(), theValue.c_str(), true);
#endif
    }

    unsigned long getThreadId() {
#ifdef _WIN32
        return GetCurrentThreadId();
#elif OSX
        return pthread_mach_thread_np(pthread_self());
#elif LINUX
        return syscall(SYS_gettid);
#else
        return pthread_self(); // for future use
#endif
    }


#ifdef _WIN32
 bool
 hResultIsOk(HRESULT hr, std::string & theMessage)
 {
     if (FAILED(hr)) {
         if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
             hr = HRESULT_CODE(hr);
         }
         char * szErrMsg;
         if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
             NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
             (LPTSTR)&szErrMsg, 0, NULL) != 0)
         {
             theMessage = szErrMsg;
             LocalFree(szErrMsg);
         } else {
             char myErrorNumber[1024];
             sprintf(myErrorNumber, "Error # %#x", hr);
             theMessage = myErrorNumber;
         }
         AC_DEBUG << "Error for hr " << hr << " : " << theMessage;
         return false;
     }
     return true;
 }

#endif

} //Namespace asl


