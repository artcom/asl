/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: UnitTest.h,v $
//
//   $Revision: 1.8 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_UnitTest_
#define _included_UnitTest_

#include "Exception.h"

#include <iostream>
#include <vector>
#include <string>


/*! \addtogroup aslbase */
/* @{ */

class UnitTest {
public:
    virtual void run() = 0;
    unsigned int getFailedCount() const; 
    unsigned int getPassedCount() const;
    const char * getMyName() const;
    virtual ~UnitTest();
    virtual void incrementFailedCount();
    virtual void incrementPassedCount();
    int returnStatus() const;
    const char * getTracePrefix ();
    virtual void setSilentSuccess(bool makeSilent = true);
    virtual void setAbortOnFailure(bool makeAbort = true);
    virtual void setup();
    virtual void teardown();
 private:
    UnitTest();
    UnitTest(const UnitTest &);
    const UnitTest & operator=(const UnitTest &);
protected:
    explicit UnitTest(const char * myName);
public:
    void ensure(bool myExpressionResult,
              const char * myExpression,
              const char * mySourceFileName,
              unsigned long mySourceLine);
protected:
    virtual void setFailedCount(unsigned int failedTests);
    virtual void setPassedCount(unsigned int passedTests);
    virtual void setMyName(const char * theName);
    friend class UnitTestSuite;
private:
    const char * _myName;
    unsigned int _passedCount;
    unsigned int _failedCount;
    bool _silentSuccess;
    bool _abortOnFailure;
};

class UnitTestSuite : public UnitTest {
public:
    explicit UnitTestSuite(const char * myName) : UnitTest(myName) {
    }
    ~UnitTestSuite() {
        destroyMyTests();
    }

    virtual void addTest(UnitTest * myTest) {
        if (myTest) {
            _myTests.push_back(myTest);
            std::cerr << ">> Added Test '" << myTest->getMyName() << "', ptr = " << myTest << std::endl;
        } else {
            std::cerr << "## Failed to add Test" << std::endl;
            incrementFailedCount();
        }
    }
    virtual void run();
    
    virtual void setup() {
        std::cerr << ">> Launching Test Suite '" << getMyName() << "'" << std::endl;
    }
    virtual void teardown();
protected:
    int getNumTests() const {
        return _myTests.size();
    }
    UnitTest * getTest(int i) {
        return _myTests[i];
    }
private:
    void destroyMyTests() {
        while (_myTests.size()) {
            delete _myTests.back();
            _myTests.pop_back();
        }
    }
    std::vector<UnitTest *> _myTests;
};

class TemplateUnitTest : public UnitTest {
public:
    TemplateUnitTest(const char * theClassName, const char * theTemplateArgument)
        : UnitTest(strdup(std::string(std::string(theClassName) + theTemplateArgument).c_str())) {  }
    ~TemplateUnitTest() {
        free((char*)getMyName());
    }
};

template <int N, class TEST>
class Repeat : public TEST {
public:
    Repeat() { }

    template <class ARG>
    Repeat(ARG theArgument) : TEST(theArgument) { }

    void run() {
            std::cerr << ">>>>>>>> Repeating Test " << N << " times,  this = " << this << std::endl;
            for (int i = 0; i < N; ++i) {
                TEST::run();
            }
        };
};



#define ENSURE(EXP) ensure(EXP, # EXP, __FILE__, __LINE__);
#define ENSURE_MSG(EXP, MSG) ensure(EXP, MSG, __FILE__, __LINE__);
#define ENSURE_EXCEPTION(theTest, theException) \
  { \
    try { \
        theTest; \
        FAILURE(#theTest) \
    } catch (const theException &) { \
        SUCCESS(#theTest) \
    } \
 }

#define FAILURE(MSG) ensure(false, MSG, __FILE__, __LINE__);
#define SUCCESS(MSG) ensure(true, MSG, __FILE__, __LINE__);
#define DTITLE(TITLE) std::cerr << getTracePrefix() << TITLE << ":" << std::endl;;
#define DPRINT(VARIABLE) std::cerr << getTracePrefix() << #VARIABLE << " = " << VARIABLE << std::endl;;
#define DDUMP(MULTI_LINE_VARIABLE) std::cerr << getTracePrefix() << #MULTI_LINE_VARIABLE<< ":" << std::endl << MULTI_LINE_VARIABLE<< std::endl;
#define DPRINT2(TITEL,VARIABLE) std::cerr << getTracePrefix() << TITEL << ": " << #VARIABLE << " = " << VARIABLE << std::endl;;


/* @} */

#endif
