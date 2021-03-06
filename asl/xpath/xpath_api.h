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

/*!
** \file xpath/xpath_api.h
**
** \brief XPath API
**
** \author Hendrik Schober
**
** \date 2008-07-15
**
*/

/**********************************************************************************************/

#if !defined(XPATH_API_H)
#define XPATH_API_H

#include "asl_xpath_settings.h"

/**********************************************************************************************/
#include <vector>
/**********************************************************************************************/
#include <asl/dom/Nodes.h>
/**********************************************************************************************/
#include "XPathPath.h"

/**********************************************************************************************/

namespace xpath {

    typedef std::vector<dom::NodePtr>           NodeList;

    ASL_XPATH_DECL void find(const Path&, const dom::NodePtr, dom::NodePtr&);

    ASL_XPATH_DECL dom::NodePtr find(const Path&, const dom::NodePtr);

    ASL_XPATH_DECL void findAll(const Path&, const dom::NodePtr, NodeList&);

    ASL_XPATH_DECL NodeList findAll(const Path&, const dom::NodePtr);

    inline
    dom::NodePtr
    find(const Path& thePath, const dom::NodePtr theNode)
    {
        dom::NodePtr myResult;
        find(thePath,theNode,myResult);
        return myResult;
    }

    inline
    NodeList
    findAll(const Path& thePath, const dom::NodePtr theNode)
    {
        NodeList myResult;
        findAll(thePath,theNode,myResult);
        return myResult;
    }

}

/**********************************************************************************************/

#endif //!defined(XPATH_API_H)
