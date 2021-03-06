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
*/
//
//    $RCSfile: VectorManipulator.h,v $
//
//    $Author: david $
//
//    $Revision: 1.2 $
//
//=============================================================================


#ifndef ASL_VECTOR_MANIPULATOR_INCLUDED
#define ASL_VECTOR_MANIPULATOR_INCLUDED

#include "asl_math_settings.h"

#include <iostream>
#include <ios>

namespace asl {

class ASL_MATH_DECL FixedVectorStreamFormatter {
    public:
        FixedVectorStreamFormatter(const char theStartToken,
                                   const char theEndToken,
                                   const char theDelimiter,
                                   bool theOneElementPerLineFlag);
        FixedVectorStreamFormatter( const FixedVectorStreamFormatter & otherFormatter);

        void setStreamInfo(std::ios_base & theStream) const;


        static const int ourIsFormattedFlag;
        static const int ourStartTokenIndex;
        static const int ourEndTokenIndex;
        static const int ourDelimiterIndex;
        static const int ourOneElementPerLineFlagIndex;



    private:
        FixedVectorStreamFormatter();
        FixedVectorStreamFormatter & operator=(const FixedVectorStreamFormatter & otherFormatter);

        const char _myStartToken;
        const char _myEndToken;
        const char _myDelimiter;
        bool       _myOneElementPerLineFlag;
};

ASL_MATH_DECL FixedVectorStreamFormatter
formatFixedVector(const char theStartToken,
                  const char theEndToken,
                  const char theDelimiter,
                  bool theOneElementPerLineFlag);

ASL_MATH_DECL std::ostream &
operator << (std::ostream & theStream, const FixedVectorStreamFormatter & theFormatter);

ASL_MATH_DECL std::istream &
operator >> (std::istream & theStream, const FixedVectorStreamFormatter & theFormatter);

}

#endif // ASL_VECTOR_MANIPULATOR_INCLUDED
