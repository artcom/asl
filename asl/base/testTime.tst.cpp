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
//    $RCSfile: testTime.tst.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: unit test template file - change Time to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "Time.h"
#include "TimeStreamFormater.h"
#include "os_functions.h"
#include <string>
#include <iostream>

using namespace std;

class TimeParserTest : public UnitTest {
public:
    TimeParserTest() : UnitTest("TimeParserTest") {  }
    void testParser(const std::string & theString, int theSecs) {
        asl::Time myParsedTime;
        myParsedTime.parse(theString);
        DPRINT(myParsedTime);
        ENSURE_EQUAL(myParsedTime, asl::Time(double(theSecs)));
    }
    void run() {
        int mySecs = 1154528186;
        //unsetenv("TZ");
        tzset();
        testParser("Wed, 02 Aug 2006 14:16:26 GMT", mySecs);
        testParser("Wed, 02 Aug 2006 15:16:26 CET", mySecs);
        testParser("Wed, 02 Aug 2006 16:16:26 CEST", mySecs);
        asl::set_environment_var("TZ", "UTC");
        tzset();
        testParser("Wed, 02 Aug 2006 14:16:26 GMT", mySecs);
        testParser("Wed, 02 Aug 2006 15:16:26 CET", mySecs);
        testParser("Wed, 02 Aug 2006 16:16:26 CEST", mySecs);
        asl::set_environment_var("TZ", "MEZ");
        tzset();
        testParser("Wed, 02 Aug 2006 14:16:26 GMT", mySecs);
        testParser("Wed, 02 Aug 2006 15:16:26 CET", mySecs);
        testParser("Wed, 02 Aug 2006 16:16:26 CEST", mySecs);
        asl::set_environment_var("TZ", "MESZ");
        tzset();
        testParser("Wed, 02 Aug 2006 14:16:26 GMT", mySecs);
        testParser("Wed, 02 Aug 2006 15:16:26 CET", mySecs);
        testParser("Wed, 02 Aug 2006 16:16:26 CEST", mySecs);
        asl::set_environment_var("TZ", "UTC+2");
        tzset();
        testParser("Wed, 02 Aug 2006 14:16:26 GMT", mySecs);
        testParser("Wed, 02 Aug 2006 15:16:26 CET", mySecs);
        testParser("Wed, 02 Aug 2006 16:16:26 CEST", mySecs);
    }
};

class TimeUnitTest : public UnitTest {
public:
    TimeUnitTest() : UnitTest("TimeUnitTest") {  }

    void run() {
        cout << "hej!" << endl;
        asl::Time now;
        asl::Time later;
        cout << "now: " << now << endl;
        cout << "later: " << later << endl;
        ENSURE(later >= now);
        asl::Time lnow;
        asl::Time llater;
        lnow.toLocalTime();
        llater.toLocalTime();
        cout << "local now: " << lnow << endl;
        cout << "local later: " << llater << endl;
        ENSURE(later >= now);
        double nowd = now;
        asl::Time nowdd = nowd;
        ENSURE(fabs(now - nowd) < 1e-3);
        ENSURE(fabs(now - nowdd) < 1e-3);
        ENSURE(asl::Time(1) == asl::Time(1));
        asl::Time now2 = now;
        ENSURE(now2==now);
        ENSURE(now2>=now);
        ENSURE(now2<=now);
        now2 = now + 1;
        ENSURE(now2!=now);
        ENSURE(now2>now);
        ENSURE(now<now2);
        ENSURE(now2>=now);
        ENSURE(now<=now2);

        now = asl::Time(1.0401);
        ENSURE(now.secs() == 1);
        ENSURE(now.millis() == 1040);
        ENSURE(now.micros() == 1040100);
        ENSURE(now.usecs() == 40100);
        DPRINT(now.usecs());
        now2 = asl::Time(1, 40100);
        DPRINT(now2);
        ENSURE(now==now2);

        double start = asl::Time();
        asl::msleep(100);
        double stop = asl::Time();
        ENSURE(stop-start > 0.09);
        ENSURE(stop-start < 0.2);
        cerr << "start: " << start << ", stop: " << stop << endl;
        cerr << "stop-start: " << stop-start << endl;

        now = asl::Time();
        ENSURE(static_cast<int>(now.millis()/1000) == now.secs());
        ENSURE(static_cast<int>(now.micros()/1000000) == now.secs());

        now = asl::Time();
        asl::msleep(100);
        later = asl::Time();
        cerr << "now:   "<<"usecs="<<now.usecs()<<", micros="<<now.micros()<<", millis="<<now.millis()<<", secs="<<now.secs() << endl;
        cerr << "later: "<<"usecs="<<later.usecs()<<", micros="<<later.micros()<<", millis="<<later.millis()<<", secs="<<later.secs() << endl;
        ENSURE(later > now);
        ENSURE(later.micros() > now.micros());
        ENSURE(later.millis() > now.millis());

        cerr << "difference in millis:  " << (later.millis() - now.millis()) << endl;
        cerr << "difference in micros:  " << (later.micros() - now.micros()) << endl;
        
        const char * myFormatString("%Y-%M-%D-%h:%m:%s.%u");
        cerr << "formatted '" << myFormatString << "': "
             << asl::formatTime(myFormatString) << now << endl;

    }
};
class NanoTimeUnitTest : public UnitTest {
public:
    NanoTimeUnitTest() : UnitTest("NanoTimeUnitTest") {  }
    void run() {
        cout << "nano!" << endl;
        asl::NanoTime now;
        asl::NanoTime later;
        cout << "per second: " << asl::NanoTime::perSecond() << endl;
        cout << "now:   ticks=" << now.ticks()<<", nsecs="<<now.nanos()<<", usecs="<<now.micros()<<", msecs="<<now.millis()<<",secs="<<now.seconds() << endl;
        cout << "later: ticks=" << later.ticks()<<", nsecs="<<later.nanos()<<", usecs="<<later.micros()<<", msecs="<<later.millis()<<",secs="<<later.seconds() << endl;
        cout << "later: " << later.ticks() << endl;
        ENSURE(later.ticks() > now.ticks());

        cerr << "difference in seconds: " << (later.secs() - now.secs()) << endl;
        cerr << "difference in millis:  " << (later.millis() - now.millis()) << endl;
        cerr << "difference in micros:  " << (later.micros() - now.micros()) << endl;
        cerr << "difference in nanos:   " << (later.nanos() - now.nanos()) << endl;
        cerr << "difference in ticks:   " << (later.ticks() - now.ticks()) << endl;

        // I assume that now and later may not be more than 100 micro sec appart
        ENSURE((later.secs() - now.secs()) < 0.0001);
        ENSURE((later.millis() - now.millis()) < 0.1);
        ENSURE((later.micros() - now.micros()) < 100);
        ENSURE((later.nanos() - now.nanos()) < 100000);
        ENSURE((later.ticks() - now.ticks()) < asl::NanoTime::perSecond() * 0.0001);

    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new TimeParserTest);
        addTest(new TimeUnitTest,100);

        // Note: the CPU's TSC is not reliable on multi-core systems, but the new HPEC is not yet widely
        // available, so we now simply use gettimeofday. This only has usec (not nsec) resolution
        // so disable the nsec test

        //addTest(new NanoTimeUnitTest,100);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
