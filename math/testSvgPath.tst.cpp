//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "SvgPath.h"

#include <asl/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;

class SvgPathTest : public UnitTest {
public:
    SvgPathTest() : UnitTest("SvgPath") {
    }

    void run() {

        {
            SvgPath myPath;
            myPath.move(Vector3f(0,10,0));
            myPath.line(Vector3f(0,20,0), true);
            ENSURE(myPath.getNumElements() == 1);
            ENSURE(myPath.getLength() == 20.0f);
        }

        {
            struct {
                char * definition;
                unsigned numElements;
            } myPaths[] = {
                { "M 0 0 l 0 50 l 10 50 l -10 50 l -50 0 z", 5 },
                { "M 0 0 h 100 v 100 Z", 3 },
                { "M 50 40 l 50 50 l 30 -20 Z", 3 },
            };

            for (unsigned i = 0; i < sizeof(myPaths) / sizeof(myPaths[0]); ++i) {
                SvgPath myPath(myPaths[i].definition);
                DPRINT(myPath.getNumElements());
                if (myPaths[i].numElements) {
                    ENSURE(myPath.getNumElements() == myPaths[i].numElements);
                }
            }
        }

        {
            SvgPath myPath("M0,0h100l50,50");
            ENSURE(almostEqual(myPath.getLength(), 170.71067f));

            DPRINT(myPath.nearest(Vector3f(42,2.3,0)));
            ENSURE(myPath.nearest(Vector3f(42,2.3,0)) == Vector3f(42,0,0));

            SvgPath::PathNormal myNormal = myPath.normal(Vector3f(130,20,0));
            ENSURE(myNormal.normal == normalized(Vector3f(-1,1,0)));
            ENSURE(myNormal.nearest == Vector3f(125,25,0));

            SvgPath * myPerPath = myPath.createPerpendicularPath(Vector3f(23,0,0), 100.0f);
            ENSURE(myPerPath->getLength() == 100.0f);
            ENSURE(myPerPath->pointOnPath(0.0f) == Vector3f(23,0,0));
            ENSURE(myPerPath->pointOnPath(0.5f) == Vector3f(23,50,0));
            ENSURE(myPerPath->pointOnPath(1.0f) == Vector3f(23,100,0));

            SvgPath * mySubPath = myPath.createSubPath(Vector3f(10,0,0), Vector3f(90,0,0));
            DPRINT(mySubPath->getLength());
            ENSURE(mySubPath->getLength() == 80.0f);
        }
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {
    }

    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new SvgPathTest());
    }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
	std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;
    return mySuite.returnStatus();
}
