//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Context.h"
#include "Expression.h"

#include "xpath_api.h"

namespace xpath {

    // defined in parser.cpp, for which no Header file exists.
    int parsePath(Path *p, const std::string &instring, int pos);

    Path *
    xpath_parse(const std::string &instring) {
        Path *p = new Path();
        AC_DEBUG << "parsing path " << instring;
        if (parsePath(p, instring, 0) == 0) {
            // parse error
            AC_WARNING << "parse error. Intermediate result=" << *p;
            delete p;
            return NULL;
        }
        AC_DEBUG << "parsing result = " << *p;
        return p;
    }

    dom::Node *
    xpath_evaluate1(Path *p, dom::Node *theNode) {

        OrderedNodeSetRef retval = p->evaluateAs<OrderedNodeSet>(theNode);

#ifdef DEBUG_RESULTS
        AC_DEBUG << "evaluated path contains " << retval->size() << " nodes.";

        for (OrderedNodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
            try {
                AC_TRACE << " * " << (*i)->nodeName() << " "
                    << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
                if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
                    for (int j = 0; j < (*i)->attributesLength(); j++) {
                        AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
                    }
                }
            } catch(asl::Exception &e) {
                AC_TRACE << " oops...";
            }
        }
#endif
        if (retval->begin() != retval->end()) {
            NodeRef retval1 = *(retval->begin());
            delete retval;
            return retval1;
        } else {
            delete retval;
            return NULL;
        }
    }

    std::vector<dom::Node *> *
    xpath_evaluate(Path *p, dom::Node *theNode) {
        xpath::NodeVectorRef retval = p->evaluateAs<NodeVector>(theNode);
#ifdef DEBUG_RESULTS
        AC_DEBUG << "evaluated path contains " << retval->size() << " nodes.";

        for (std::vector<dom::Node *>::iterator i = retval->begin(); i != retval->end(); ++i) {
            try {
                AC_TRACE << " * " << (*i)->nodeName() << " "
                    << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
                if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
                    for (int j = 0; j < (*i)->attributesLength(); j++) {
                        AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
                    }
                }
            } catch(asl::Exception &e) {
                AC_TRACE << " oops...";
            }
        }
#endif
        return retval;
    }

    std::vector<dom::Node *> *
    xpath_evaluate(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
            NodeVectorRef retval = xpath_evaluate(myPath, theNode);
            delete myPath;
            return retval;
        }
    }

    std::set<dom::Node *> *
    xpath_evaluateSet(xpath::Path *myPath, dom::Node *theNode) {
        NodeSetValue *retValue = myPath->evaluate(theNode);
        NodeSetRef retval = retValue->takeNodes();
#ifdef DEBUG_RESULTS
        AC_DEBUG << "evaluated path contains " << retval->size() << " nodes.";

        for (NodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
            try {
                AC_TRACE << " * " << (*i)->nodeName() << " "
                    << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
                if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
                    for (int j = 0; j < (*i)->attributesLength(); j++) {
                        AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
                    }
                }
            } catch(asl::Exception &e) {
                AC_TRACE << " oops...";
            }
        }
#endif
        delete retValue;
        return retval;
    }

    std::set<dom::Node *> *
    xpath_evaluateSet(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
            NodeSetRef retval = xpath_evaluateSet(myPath, theNode);
            delete myPath;
            return retval;
        }
    }

    OrderedNodeSetRef 
    xpath_evaluateOrderedSet(xpath::Path *myPath, dom::Node *theNode) {
        OrderedNodeSetRef retval = myPath->evaluateAs<OrderedNodeSet>(theNode);
#ifdef DEBUG_RESULTS
        AC_DEBUG << "evaluated path contains " << retval->size() << " nodes.";

        for (OrderedNodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
            try {
                AC_TRACE << " * " << (*i)->nodeName() << " "
                         << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
                if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
                    for (int j = 0; j < (*i)->attributesLength(); j++) {
                        AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
                    }
                }
            } catch(asl::Exception &e) {
                AC_TRACE << " oops...";
            }
        }
#endif
        return retval;
    }

    OrderedNodeSetRef
    xpath_evaluateOrderedSet(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
            OrderedNodeSetRef retval = xpath_evaluateOrderedSet(myPath, theNode);
            delete myPath;
            return retval;
        }
    }

    void
    xpath_return(Path *p) {
        delete p;
    }

    void
    xpath_evaluate(Path *p, dom::Node *startingElement, std::vector<dom::NodePtr> &results) {

        std::vector<dom::Node *> *retval = xpath_evaluate(p, startingElement);

        for (std::vector<dom::Node *>::iterator i = retval->begin(); i != retval->end(); ++i) {
            results.push_back((*i)->self().lock());
        }

        delete retval;
    }

    void
    xpath_evaluate(std::string p, dom::Node *startingElement, std::vector<dom::NodePtr> &results) {

        std::vector<dom::Node *> *retval = xpath_evaluate(p, startingElement);

        for (std::vector<dom::Node *>::iterator i = retval->begin(); i != retval->end(); ++i) {
            results.push_back((*i)->self().lock());
        }

        delete retval;
    }
   std::vector<dom::NodePtr>
   findAll(dom::Node & theRoot, const std::string & theExpression) {
        std::vector<dom::NodePtr> myResults;
        xpath::Path *myPath = xpath::xpath_parse(theExpression);
        if (!myPath) {
            AC_ERROR << "xpath could not parse '"<< theExpression<< "'";
        } else {
            xpath::xpath_evaluate(myPath, &theRoot, myResults);
            xpath::xpath_return(myPath);
        }
        return myResults;
    }

    dom::NodePtr
    find(dom::Node & theRoot, const std::string & theExpression) {
        dom::NodePtr myResult;
        xpath::Path *myPath = xpath::xpath_parse(theExpression);
        if (!myPath) {
            AC_ERROR << "xpath could not parse '"<< theExpression<< "'";
        } else {
            dom::Node * myResultNode = xpath::xpath_evaluate1(myPath, &theRoot);
            xpath::xpath_return(myPath);
            if (myResultNode) {
                myResult = myResultNode->self().lock();
            }
        }
        return myResult;
    }

}
