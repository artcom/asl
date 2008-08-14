/**********************************************************************************************/

/*
**
** Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
**
** These coded instructions, statements, and computer programs contain
** unpublished proprietary information of ART+COM AG Berlin, and
** are copy protected by law. They may not be disclosed to third parties
** or copied or duplicated in any form, in whole or in part, without the
** specific, prior written permission of ART+COM AG Berlin.
**
*/

/**********************************************************************************************/

/*!
** \file xpath/xpath_api.cpp
**
** \brief XPath API
**
** \author Hendrik Schober
**
** \date 2008-07-15
**
*/

/**********************************************************************************************/
#include "xpath_api.h"

/**********************************************************************************************/
#include <vector>
#include <set>

/**********************************************************************************************/
#include <dom/Nodes.h>

/**********************************************************************************************/
#include "Path.h"
#include "PathParser.h"

/**********************************************************************************************/

namespace {

    using namespace xpath::detail;

    /******************************************************************************************/
    /* basic stuff                                                                            */

    typedef xpath::NodeList                             NodePtrVec;

    struct Context {
        const dom::Node*            contextNode;
        NodePtrVec                  currentNodes;
        Context( const dom::Node* node )              : contextNode(node), currentNodes() {}
        Context( const dom::Node* node
               , const NodePtrVec& curr )             : contextNode(node), currentNodes(curr) {}
    };

    /******************************************************************************************/
    /* finding nodes for specific axisses                                                     */

    inline void findChildNodes(Context& context)
    {
        const dom::NodeList& children = context.contextNode->childNodes();
        const std::size_t size = children.size();
        for( std::size_t idx=0; idx<size; ++idx ) {
            const dom::Node* child = children.item(idx).getNativePtr();
            context.currentNodes.push_back(child);
        }
    }

    inline void findAttributeNodes(Context& context)
    {
        const dom::NodeList& attributes = context.contextNode->attributes();
        const std::size_t size = attributes.size();
        for( std::size_t idx=0; idx<size; ++idx ) {
            const dom::Node* attribute = attributes.item(idx).getNativePtr();
            context.currentNodes.push_back(attribute);
        }
    }

    inline void findChildNodesRecursivly(Context& context)
    {
        const dom::NodeList& children = context.contextNode->childNodes();
        const std::size_t size = children.size();
        for( std::size_t idx=0; idx<size; ++idx ) {
            const dom::Node* child = children.item(idx).getNativePtr();
            context.currentNodes.push_back(child);
            Context childcontext(child);
            findChildNodesRecursivly(childcontext);
            context.currentNodes.insert( context.currentNodes.end()
                                       , childcontext.currentNodes.begin()
                                       , childcontext.currentNodes.end  () );
        }
    }

    inline void findParentNodesRecursivly(Context& context)
    {
        const dom::Node* parent = context.contextNode->parentNode();
        if( parent ) {
            context.currentNodes.push_back(parent);
            Context parentcontext(parent);
            findParentNodesRecursivly(context);
            context.currentNodes.insert( context.currentNodes.end()
                                       , parentcontext.currentNodes.begin()
                                       , parentcontext.currentNodes.end  () );
        }
    }

    inline void findFollowingSiblingNodes(Context& context)
    {
        while( const dom::Node* const sibling = context.contextNode->nextSibling().getNativePtr() ) {
            context.currentNodes.push_back(sibling);
            Context siblingcontext(sibling);
            findChildNodesRecursivly(context);
            context.currentNodes.insert( context.currentNodes.end()
                                       , siblingcontext.currentNodes.begin()
                                       , siblingcontext.currentNodes.end  () );
        }
    }

    inline void findFollowingNodes(Context& context)
    {
        findChildNodesRecursivly(context);
        findFollowingSiblingNodes(context);
        while( const dom::Node* const parent = context.contextNode->parentNode() ) {
            Context parentcontext(parent);
            findFollowingSiblingNodes(parentcontext);
            context.currentNodes.insert( context.currentNodes.end()
                                       , parentcontext.currentNodes.begin()
                                       , parentcontext.currentNodes.end  () );
        }
    }

    inline void findPrecedingSiblingNodes(Context& context)
    {
        while( const dom::Node* const sibling = context.contextNode->previousSibling().getNativePtr() ) {
            context.currentNodes.push_back(sibling);
            Context siblingcontext(sibling);
            findChildNodesRecursivly(context);
            context.currentNodes.insert( context.currentNodes.begin()
                                       , siblingcontext.currentNodes.begin()
                                       , siblingcontext.currentNodes.end  () );
        }
    }

    inline void findPrecedingNodes(Context& context)
    {
        findChildNodesRecursivly(context);
        findPrecedingSiblingNodes(context);
        while( const dom::Node* const parent = context.contextNode->parentNode() ) {
            Context parentcontext(parent);
            findPrecedingSiblingNodes(parentcontext);
            context.currentNodes.insert( context.currentNodes.end()
                                       , parentcontext.currentNodes.begin()
                                       , parentcontext.currentNodes.end  () );
        }
    }

    inline void findAttributeChildNodes(Context& context)
    {
        findAttributeNodes(context);
    }

    /******************************************************************************************/
    /* finding nodes for a step                                                               */

    inline NodePtrVec findAxisNodes(Context& context, const Axis& axis)
    {
        Context subcontext(context.contextNode);
        switch( axis.getType() ) {
            case Axis::Type_Root             :  subcontext.currentNodes.push_back( subcontext.contextNode->getDocumentNode() );
                                                break;

            case Axis::Type_AncestorOrSelf   :  subcontext.currentNodes.push_back( subcontext.contextNode );
                                                // FALL THROUGH
            case Axis::Type_Ancestor         :  findParentNodesRecursivly(subcontext);
                                                break;

            case Axis::Type_DescendantOrSelf :  subcontext.currentNodes.push_back( subcontext.contextNode );
                                                // FALL THROUGH
            case Axis::Type_Descendant       :  findChildNodesRecursivly(subcontext);
                                                break;

            case Axis::Type_Following        :  findFollowingNodes(subcontext);
                                                break;
            case Axis::Type_FollowingSibling :  findFollowingSiblingNodes(subcontext);
                                                break;
            case Axis::Type_Preceding        :  findPrecedingNodes(subcontext);
                                                break;
            case Axis::Type_PrecedingSibling :  findPrecedingSiblingNodes(subcontext);
                                                break;

            case Axis::Type_Child            :  findChildNodes(subcontext);
                                                break;
            case Axis::Type_Parent           :  subcontext.currentNodes.push_back( subcontext.contextNode->parentNode() );
                                                break;
            case Axis::Type_Self             :  subcontext.currentNodes.push_back( subcontext.contextNode );
                                                break;
            case Axis::Type_Namespace        :  throw xpath::ParseError( "\"namespace\" axis not implemented!", PLUS_FILE_LINE );
                                                break;
            case Axis::Type_Attribute        :  findAttributeChildNodes(subcontext);
                                                break;

            case Axis::Type_Invalid          :  // FALL THROUGH
            default                          :  assert(false);
        }
        return subcontext.currentNodes;
    }

    inline const NodePtrVec findNodeTestNodes(Context& context, const NodeTest& nodeTest)
    {
        switch( nodeTest.getType() ) {
            case NodeTest::Type_NodeTest :  if( nodeTest.getValue().empty() ) {
                                                return context.currentNodes;
                                            }
                                            break;
            case NodeTest::Type_NameTest :  if( nodeTest.getValue() == "*" ) {
                                                return context.currentNodes;
                                            }
                                            break;
        }
        NodePtrVec currentNodes;
        for( NodePtrVec::const_iterator it=context.currentNodes.begin(); it!=context.currentNodes.end(); ++it ) {
            switch( nodeTest.getType() ) {
                case NodeTest::Type_CommentTest               :
                    if( (*it)->nodeType() == dom::Node::COMMENT_NODE ) {
                        currentNodes.push_back(*it);
                    }
                    break;
                case NodeTest::Type_TextTest                  :
                    if( (*it)->nodeType() == dom::Node::TEXT_NODE ) {
                        currentNodes.push_back(*it);
                    }
                    break;
            	case NodeTest::Type_ProcessingInstructionTest : 
                    if( (*it)->nodeType() == dom::Node::PROCESSING_INSTRUCTION_NODE ) {
                        if( !nodeTest.getValue().empty() ) {
                            if( nodeTest.getValue()!=(*it)->nodeValueAs<std::string>() ) {
                            }
                        }
                        currentNodes.push_back(*it);
                    }
                    break;
                case NodeTest::Type_NodeTest                  :
                    if( (*it)->nodeType() == dom::Node::ELEMENT_NODE ) {
                        currentNodes.push_back(*it);
                    }
                    break;
                case NodeTest::Type_NameTest                  :
                    if( (*it)->nodeName() == nodeTest.getValue() ) {
                        currentNodes.push_back(*it);
                    }
                    break;
                case NodeTest::Type_Invalid                   : // FALL THROUGH
                default                                       : assert(false);
            }
        }
        return currentNodes;
    }

    inline const NodePtrVec findPredicateNodes(Context& context, const Predicate& predicate)
    {
        if( predicate.empty() ) {
            return context.currentNodes;
        }
        NodePtrVec currentNodes;
        for( NodePtrVec::size_type idx=0; idx<context.currentNodes.size(); ++idx ) {
            const Predicate::EvaluationContext evContext( context.currentNodes[idx]
                                                        , context.currentNodes.size()
                                                        , idx+1 );
            if( predicate.evaluate(evContext) ) {
                currentNodes.push_back(context.currentNodes[idx]);
            }
        }
        return currentNodes;
    }

    inline void findStepNodes( Context& context, const LocationStep& step )
    {
        Context subcontext(context.contextNode);
        subcontext.currentNodes = findAxisNodes     (subcontext,step.getAxis()     );
        subcontext.currentNodes = findNodeTestNodes (subcontext,step.getNodeTest() );
        subcontext.currentNodes = findPredicateNodes(subcontext,step.getPredicate());
        context.currentNodes.insert( context.currentNodes.end()
                                   , subcontext.currentNodes.begin()
                                   , subcontext.currentNodes.end  () );
    }

    /******************************************************************************************/
    /* finding nodes for a step list                                                          */

    template< typename RanIt >
    void findNodes( Context& context
                  , RanIt begin, RanIt end
                  , NodePtrVec& result
                  , bool stopOnFirstHit )
    {
        if( begin == end ) {
            return;
        }
        findStepNodes(context,*begin);
        ++begin;
        if( begin != end ) {
            for( NodePtrVec::const_iterator it=context.currentNodes.begin(); it!=context.currentNodes.end(); ++it ) {
                Context childcontext(*it);
                findNodes(childcontext,begin,end,result,stopOnFirstHit);
                if( stopOnFirstHit && !result.empty() ) {
                    return;
                }
            }
        } else {
            for( NodePtrVec::const_iterator it=context.currentNodes.begin(); it!=context.currentNodes.end(); ++it ) {
                const NodePtrVec::const_iterator found = std::find(result.begin(),result.end(),*it);
                if( found == result.end() ) {
                    result.push_back(*it);
                }
            }
        }
    }

    /******************************************************************************************/

}

/**********************************************************************************************/

namespace xpath {

    void find(const Path& path, const dom::NodePtr node, const dom::Node*& result)
    {
        const detail::LocationStepList& steps = path.getStepList().steps;
        Context context(node.getNativePtr());
        NodePtrVec temp;
        findNodes(context,steps.begin(),steps.end(),temp,true);
        if( !temp.empty() ) {
            result = temp.front();
        }
    }

    void findAll(const Path& path, const dom::NodePtr node, NodeList& result)
    {
        const detail::LocationStepList& steps = path.getStepList().steps;
        Context context(node.getNativePtr());
        result.clear();
        findNodes(context,steps.begin(),steps.end(),result,false);
    }

}

/**********************************************************************************************/
