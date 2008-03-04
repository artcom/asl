//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef XPATH_CONTEXT_H
#define XPATH_CONTEXT_H

#include "Value.h"

namespace xpath {
    class Context {
        public:
            Context() { };
            Context(NodeRef n) { currentNode = n; position = 1; size=1; depth = 0;};
            //
            // XXX TODO: Understand this:
            // Context(const Context &other) {
            //      size = other.size;
            //      position = other.position;
            //      currentNode = other.currentNode;
            //      size = other.size + 1;
            // };
            // Why is the size member first set to other.size and later to other.size +1?
            // This looks like a bug to me, but as long as I don't understand it I'll leave 
            // it alone. [DS]
            Context( const Context & other ) : 
                size( other.size ),
                position( other.position ),
                currentNode( other.currentNode )
            {
                size = other.size + 1; // TODO: Understand the +1 [DS]
            }

            // context position
            NodeRef currentNode;

            int position;

            int size;

            int depth;
    };

};

#endif

