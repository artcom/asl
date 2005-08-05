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
//    $RCSnumeric: test_linearAlgebra.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test for linear algebra
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "intersection.h"

#include <asl/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;

class IntersectionTest : public UnitTest {
    public:
        IntersectionTest() : UnitTest("IntersectionTest") {  }
        void run() {
            //line segment to point distance test
            //    A    B        D
            // ---*----*--------*-
            //
            //             *        *
            //             C        E
            // 
            Point3f A(-1.0f, 0.0f, 0.0f);
            Point3f B(0.0f, 0.0f, 0.0f);
            Point3f C(1.0f, 1.0f, 0.0f);
            Point3f D(2.0f, 0.0f, 0.0f);
            Point3f E(3.0f, 1.0f, 0.0f);
            {
                Line<float> myLine(A,B);
                ENSURE(almostEqual(distance(myLine,C), 1.0f));
                LineSegment<float> mySegment(A,B);
                ENSURE(almostEqual(distance(mySegment,C), sqrt(2.0f)));
            }
            {
                Line<float> myLine(B,A);
                ENSURE(almostEqual(distance(myLine,C), 1.0f));
                LineSegment<float> mySegment(B,A);
                ENSURE(almostEqual(distance(mySegment,C), sqrt(2.0f)));
            }
            {
                Line<float> myLine(A,D);
                ENSURE(almostEqual(distance(myLine,C), 1.0f));
                LineSegment<float> mySegment(A,D);
                ENSURE(almostEqual(distance(mySegment,C), 1.0f));
            }
            {
                Line<float> myLine(D,B);
                ENSURE(almostEqual(distance(myLine,C), 1.0f));
                LineSegment<float> mySegment(D,B);
                ENSURE(almostEqual(distance(mySegment,C), 1.0f));
            }
            {
                LineSegment<float> mySegment(Point3f(3.4f,0.4f,0),Point3f(-6.0f,-1.2f,0));
                Point3f myPoint(-1.5f, -0.25f,0);
                Point3f myNearest = nearest(mySegment, myPoint);
                DPRINT(myNearest);
                float dist = distance(mySegment, myPoint);
                DPRINT(dist);
                ENSURE(dist < 2.0f);
            }
            {
                LineSegment<float> mySegment(A,D);
                Point3f myNearest = nearest(mySegment, E);
                DPRINT(myNearest);
                float dist = distance(mySegment, E);
                DPRINT(dist);
                ENSURE(dist < 2.0f);
            }
            //line segment to line segment intersection/distance test
            //
            // line segments intersect
            {
            LineSegment<float> mySegment1(Point3f(0.0f,0.0f,0),Point3f(1.0f,1.0f,0));
            LineSegment<float> mySegment2(Point3f(0.0f,0.5f,0),Point3f(1.0f,0.5f,0));
            float t,s;
            Point3f myIntersectionPoint;
            bool intersectFlag = intersection(mySegment1,mySegment2,myIntersectionPoint,t,s); 
            ENSURE(intersectFlag == true);
            ENSURE(almostEqual(myIntersectionPoint,Point3f(0.5f,0.5f,0)));
            }
            // line segments are parallel
            {
            LineSegment<float> mySegment1(Point3f(0.0f,0.0f,0),Point3f(1.0f,0.0f,0));
            LineSegment<float> mySegment2(Point3f(0.0f,1.0f,0),Point3f(1.0f,1.0f,0));
            float t,s;
            Point3f myIntersectionPoint;
            bool intersectFlag = intersection(mySegment1,mySegment2,myIntersectionPoint,t,s); 
            ENSURE(intersectFlag == false);
            }
            // line segments are skew
            {
            LineSegment<float> mySegment1(Point3f(0.0f,0.0f,0),Point3f(1.0f,0.0f,0));
            LineSegment<float> mySegment2(Point3f(0.0f,1.0f,0),Point3f(1.0f,1.0f,1.0));
            float t,s;
            Point3f myIntersectionPoint;
            bool intersectFlag = intersection(mySegment1,mySegment2,myIntersectionPoint,t,s); 
            ENSURE(intersectFlag == false);
            }
            // line segments are too short
            {
            LineSegment<float> mySegment1(Point3f(0.0f,0.0f,0),Point3f(1.0f,1.0f,0));
            LineSegment<float> mySegment2(Point3f(0.0f,0.5f,0),Point3f(0.1f,0.5f,0));
            float t,s;
            Point3f myIntersectionPoint;
            bool intersectFlag = intersection(mySegment1,mySegment2,myIntersectionPoint,t,s); 
            ENSURE(intersectFlag == false);
            }
            
            
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new IntersectionTest);
 
    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

	std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

