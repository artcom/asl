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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: Plane.h,v $
//
//     $Author: danielk $
//
//   $Revision: 1.17 $
//
// Description: Plane class
//
//=============================================================================

#ifndef _included_asl_Plane_
#define _included_asl_Plane_

#include "asl_math_settings.h"

#include "Vector234.h"
#include "Point234.h"

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    // Note: use Triangle to construct a plane from three points
	template<class Number>
	class Plane {
	public:
        // Note: offset is the negative distance of the plane from the origin; it is the number you
        // have to add to move the plane into the origin.
		Plane(){};
        Plane(const Plane<Number> & p)
            : normal(p.normal), offset(p.offset) {}
		Plane(const Vector3<Number> & theNormal, Number theOffset)
            : normal(theNormal), offset(theOffset) {}
        Plane(const Vector3<Number> & theNormal, const Point3<Number> & thePointOnPlane) :
            normal(theNormal)
        {
            Vector3<Number> myProduct = normal;
            myProduct.mult(asVector(thePointOnPlane));
            offset = - (myProduct[0] + myProduct[1] + myProduct[2]);
        }
        void normalize() {
            normal = normalized(normal);
        }
        void setNormal(const Vector3<Number> & theNormal) {
            normal = theNormal;
        }
        void setOffset(Number theOffset) {
            offset = theOffset;
        }

        Plane<Number> & operator=(const Plane<Number> & otherPlane) {
            if (this != & otherPlane) {
                normal = otherPlane.normal;
                offset = otherPlane.offset;
            }
            return * this;
        }

        template<class T>
        Vector4<T>
        getCoefficients() const {
            return Vector4<T>(normal[0], normal[1], normal[2], offset);
        }

    public:
		Vector3<Number> normal; // a vector perpendicular to the plane
		Number offset;  // distance from the origin
	};

    template<class Number>
	Vector3<Number> projection(const Vector3<Number> & d, const Plane<Number> & j) {
		return d - (dot(d,j.normal) * j.normal);
	}

	//nearest point to p on a plane p
    template<class Number>
	Point3<Number> nearest(const Point3<Number> & p, const Plane<Number> & j) {
		return  p - ((j.offset + dot(j.normal,asVector(p)))/dot(j.normal,j.normal)) * j.normal;
	}
    template<class Number>
	Point3<Number> nearest(const Plane<Number> & j, const Point3<Number> & p) {
        return nearest(p, j);
    }

    template<class Number>
	Number distance(const Point3<Number> & p, const Plane<Number> & j) {
		return distance(p, nearest(p,j));
	}

    template<class Number>
	Number signedDistance(const Point3<Number> & p, const Plane<Number> & j) {
		return dot(asVector(p), j.normal) + j.offset;
	}

    template<class Number>
	Number distance(const Plane<Number> & j, const Point3<Number> & p) {
		return distance(p, j);
	}

    typedef Plane<float> Planef;
    typedef Plane<double> Planed;

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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
    template <class Number>
    std::ostream & printPlane (std::ostream & os, const Plane<Number> & myPlane,
            bool oneElementPerLine = false,
            const char theStartToken = '[',
            const char theEndToken = ']',
            const char theDelimiter = ',')

    {
            os << theStartToken;
            asl::printVector(os, myPlane.normal, oneElementPerLine, theStartToken, theEndToken, theDelimiter);
            os << theDelimiter;
            os << myPlane.offset;
            os << theEndToken;
            return os;
    }

    template <class Number>
    std::ostream & operator << (std::ostream & os, const Plane<Number> & thePlane) {
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printPlane(os, thePlane,
                    0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return printPlane(os, thePlane);
        }
    }

    template <class Number>
    std::istream & parsePlane(std::istream & is, Plane<Number> & p,
                           const char theStartToken = '[',
                           const char theEndToken = ']',
                           const char theDelimiter = ',')
    {
        char myChar;
        if ((is >> myChar)) {
            if (myChar == theStartToken) {
                if (asl::parseVector(is, p.normal, theStartToken, theEndToken, theDelimiter)) {
                    if ((is >> myChar)) {
                        if (myChar == theDelimiter) {
                            if (is >> p.offset) {
                                if (is >> myChar) {
                                    if (myChar == theEndToken) {
                                        return is;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        is.setstate(std::ios::failbit);
        return is;
    }

    template <class T>
    std::istream & operator>>(std::istream & is, Plane<T> & thePlane) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parsePlane(is, thePlane, static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)));
        } else {
            return parsePlane(is, thePlane);
        }
    }

    /* @} */
}
#endif

