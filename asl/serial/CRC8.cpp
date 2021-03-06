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
//    $RCSfile: CRC8.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
//=============================================================================


//own header
#include "CRC8.h"


namespace asl {

char crc8(void *buf, int size )
{
    int i,j;
    char result = 0;
    char *p = (char *)buf;

    for ( i = 0 ; i < size ; i++ )
    {
        result ^= *p++;
        for ( j = 0 ; j < 8; j++ )
        {
            if ( result & 0x80 )
            result = (result << 1) ^ 7;
            else
            result <<= 1;
        }
    }
    return result;
}

char crc8i(char init, void *buf, int size )
{
    int i, j;
    char result;
    char *p = (char*)buf;

    result = crc8(&init, 1);

    for ( i = 0; i < size; i++ )
    {
        result ^= *p++;
        for ( j = 0 ; j < 8; j++ )
        {
            if ( result & 0x80 )
            result = (result << 1) ^ 7;
            else
            result <<= 1;
        }
    }
    return result;
}

}
