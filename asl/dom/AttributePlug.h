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
//   $RCSfile: AttributePlug.h,v $
//
//   $Revision: 1.15 $
//
//   Description:
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_AttributePlug_h_
#define _xml_AttributePlug_h_

#include "asl_dom_settings.h"

#include "typedefs.h"
#include "Nodes.h"
#include "Value.h"
#include "PlugHelper.h"
#include "PropertyPlug.h"

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

#if defined(_WIN32)
#   define ASL_DOM_EXPORT_STATICS(thePlugName,theTagName,theExportToken) template class theExportToken thePlugName<theTagName>;
#else
#   define ASL_DOM_EXPORT_STATICS(thePlugName,theTagName,theExportToken)
#endif

#define ATTRIBUTE_TAG_DEFAULT_VALUE(theType) \
    static const dom::ValuePtr getDefaultValue() { \
        static dom::ValuePtr myValue(new dom::ValueWrapper<theType>::Type(getDefault(), 0)); \
        return myValue; \
    }

#define ATTRIBUTE_TAG_NO_DEFAULT_VALUE() \
    static const dom::ValuePtr getDefaultValue() { \
        return dom::ValuePtr(); \
    }

#define DEFINE_ATTRIBUTE_TAG(theTagName, theType, theAttributeName, theDefault, theExportToken) \
    struct theTagName { \
        typedef theType TYPE; \
        static const char * getName() { return theAttributeName; } \
        static const TYPE getDefault() { return theDefault; } \
        ASL_DOM_EXPORT_STATICS(dom::AttributePlug,theTagName,theExportToken) \
        typedef dom::AttributePlug<theTagName> Plug; \
        ATTRIBUTE_TAG_DEFAULT_VALUE(theType); \
    };

#define DEFINE_FACADE_ATTRIBUTE_TAG(theTagName, theType, theAttributeName, theDefault, theExportToken) \
    struct theTagName { \
        typedef theType TYPE; \
        static const char * getName() { return theAttributeName; } \
        static const TYPE getDefault() { return theDefault; } \
        ASL_DOM_EXPORT_STATICS(dom::FacadeAttributePlug,theTagName,theExportToken) \
        typedef dom::FacadeAttributePlug<theTagName> Plug; \
        ATTRIBUTE_TAG_DEFAULT_VALUE(theType) \
    };

    class Connector {
    public:
        template <class THE_FIRST, class THE_SECOND>
        static void dependsOn(THE_FIRST & theFirst, THE_SECOND & theSecond) {
            theFirst.dependsOn(theSecond.getValuePtr());
        }
        template <class THE_FIRST, class THE_SECOND>
        static void noLongerDependsOn(THE_FIRST & theFirst, THE_SECOND & theSecond) {
            theFirst.noLongerDependsOn(theSecond.getValuePtr());
        }
        template <class THE_FIRST, class THE_SECOND>
        static bool alreadyDependsOn(THE_FIRST & theFirst, THE_SECOND & theSecond) {
            return theFirst.alreadyDependsOn(theSecond.getValuePtr());
        }
        template <class THE_FIRST, class THE_SECOND>
        static void affects(THE_FIRST & theFirst, THE_SECOND & theSecond) {
            theSecond.dependsOn(theFirst.getValuePtr());
        }
        template <class THE_FIRST, class THE_SECOND>
        static void noLongerAffects(THE_FIRST & theFirst, THE_SECOND & theSecond) {
            theSecond.noLongerAffects(theFirst.getValuePtr());
        }
        template <class THE_FIRST, class THE_SECOND>
        static bool alreadyAffects(THE_FIRST & theFirst, THE_SECOND & theSecond) {
            return theSecond.alreadyDependsOn(theFirst.getValuePtr());
        }
    };

    template<class TAG>
    class AttributePlug : PlugBase {
    public:
        friend class Connector;
        typedef typename TAG::TYPE VALUE;
        typedef typename ValueWrapper<VALUE>::Type WRAPPER;

        AttributePlug(const Node & theNode) : _myAttribute(ensureAttribute(theNode)) { }
        AttributePlug(NodePtr theAttribute) : _myAttribute(theAttribute) { _myAttribute->setPlug(this); }
        virtual ~AttributePlug() {}

        const ValuePtr getDefaultValue(const Node & theNode) const {
            if(!_myAttribute) {
                _myAttribute = ensureAttribute(theNode);
            }
            return TAG::getDefaultValue();
        }

        ValuePtr getValuePtr() {
            return _myAttribute->nodeValueWrapperPtr();
        }

        const ValuePtr getValuePtr() const {
            return _myAttribute->nodeValueWrapperPtr();
        }

        const VALUE & getValue(const Node & theNode) const {
            if (!_myAttribute) {
                _myAttribute = ensureAttribute(theNode);
            }
            const VALUE & myReturnValue =
                ValueHelper<VALUE, WRAPPER>::getValue(_myAttribute->nodeValueWrapperPtr());

            if (updateBeforeRead(const_cast<VALUE&>(myReturnValue))) {
                _myAttribute->setVersion(theNode.documentVersion());
            }
            return myReturnValue;
        }

        unsigned long long getVersion(const Node & theNode) const {
            if (!_myAttribute) {
                _myAttribute = ensureAttribute(theNode);
            }
            return _myAttribute->nodeVersion();
        }

        const VALUE & setValue(Node & theNode, const VALUE & theValue) {
            if (!_myAttribute) {
                _myAttribute = theNode.getAttribute(TAG::getName());
                if (!_myAttribute) {
                    _myAttribute = const_cast<Node&>(theNode).appendAttribute(TAG::getName(), theValue);
                }
                ensureSchemaVsFacade(theNode, _myAttribute);
            }
            const VALUE & newValue = ValueHelper<VALUE, WRAPPER>::setValue(_myAttribute->nodeValueWrapperPtr(), theValue);
            // if we are a facade attribute, set the attribute's version to the version
            // of the dom document
            if (_myAttribute->parentNode() != &theNode) {
                _myAttribute->setVersion(theNode.documentVersion());
            }
            return newValue;
        }

#ifdef DEBUG_VARIANT
        void debug() const {
            getValuePtr()->printPrecursorGraph();
            getValuePtr()->printDependendGraph();
        }
#endif

    protected:
        bool hasOutdatedDependencies() const {
            if (_myAttribute) {
                return _myAttribute->nodeValueWrapperPtr()->hasOutdatedDependencies();
            }
            return true;
        }
        void ensureDependencies() const {
            if (_myAttribute && _myAttribute->nodeValueWrapperPtr()) {
                _myAttribute->nodeValueWrapperPtr()->ensureDependencies();
            }
        }


        template <class THE_OTHER_TAG>
        void affects(AttributePlug<THE_OTHER_TAG> & theOtherTag) {
            Connector::affects(*this, theOtherTag);
        }
        template <class THE_OTHER_TAG>
        void noLongerAffects(AttributePlug<THE_OTHER_TAG> & theOtherTag) {
            Connector::noLongerAffects(*this, theOtherTag);
        }
        template <class THE_OTHER_TAG>
        bool alreadyAffects(AttributePlug<THE_OTHER_TAG> & theOtherTag) {
            return Connector::alreadyAffects(*this, theOtherTag);
        }

        template <class CALCULATOR>
        void setCalculatorFunction(asl::Ptr<CALCULATOR, ThreadingModel> theCalculator, void (CALCULATOR::*theCalculateFunction)()) {
            getValuePtr()->setCalculatorFunction(theCalculator, theCalculateFunction);
        }
        template <class CALCULATOR>
        void setCalculatorFunction(void (CALCULATOR::*theCalculateFunction)()) {
            FacadePtr mySelf = dynamic_cast<CALCULATOR*>(this)->getSelf();
            getValuePtr()->setCalculatorFunction(dynamic_cast_Ptr<CALCULATOR>(mySelf), theCalculateFunction);
        }
        template <class CONNECTOR>
        void setReconnectFunction(void (CONNECTOR::*theConnectFunction)()) {
            FacadePtr mySelf = dynamic_cast<CONNECTOR*>(this)->getSelf();
            getValuePtr()->setReconnectFunction(CallBackPtr(
                new CallBack<CONNECTOR>(dynamic_cast_Ptr<CONNECTOR>(mySelf), theConnectFunction)));
        }

        virtual bool updateBeforeRead(VALUE & theValue) const {
            return false;
        };

        NodePtr ensureAttribute(const Node & theNode) const {
            if (!theNode) {
                return NodePtr(); // to allow factory nodes
            }
            NodePtr myAttribute = theNode.getAttribute(TAG::getName());
            if (!myAttribute) {
                try {
                    myAttribute = const_cast<Node&>(theNode).appendAttribute(TAG::getName(), TAG::getDefault());
                } catch (MissingAttributeException) {
                    throw Facade::Exception(std::string("Attribute '") + TAG::getName() +
                        "' not found in node:\n" + asl::as_string(theNode), PLUS_FILE_LINE);
                }
            }
            myAttribute->setPlug(this);
            ensureSchemaVsFacade(theNode, myAttribute);
            return myAttribute;
        }
        mutable NodePtr _myAttribute;
    private:
        AttributePlug() {};

        static void ensureSchemaVsFacade(const Node & theNode, NodePtr theAttribute) {
            if (!ValueHelper<VALUE, WRAPPER>::checkValueType(theAttribute->nodeValueWrapperPtr())) {
                AC_ERROR << "*nodeValueWrapperPtr :" << typeid(*theAttribute->nodeValueWrapperPtr()).name();
                AC_ERROR << "nodeValueWrapperPtr :" << typeid(theAttribute->nodeValueWrapperPtr()).name();
                AC_ERROR << "WRAPPER :" << typeid(WRAPPER).name();
                throw SchemaVsFacadeTypeMismatch(std::string("Attribute '") + TAG::getName() +
                    "' has incompatible schema type in node:\n" +
                    asl::as_string(theNode), PLUS_FILE_LINE);
            }
        }
    };

    // a candidate for the obfuscated C++ contest :-)
    template<class TAG>
    class FacadeAttributePlug : public AttributePlug<TAG> {
        friend class Connector;
    protected:
        FacadeAttributePlug(NamedNodeMap * theFacade) :
             AttributePlug<TAG>(
                 theFacade->append(
                 NodePtr(new Attribute(TAG::getName(),
                 dom::ValuePtr(
                 new typename dom::ValueWrapper<typename TAG::TYPE>::Type(TAG::getDefault(),0)
                 )
                 ,0))
                 )
                 )
             {
                 this->getValuePtr()->setSelf(this->getValuePtr());
             }

             template <class THE_OTHER_TAG>
             void dependsOn(AttributePlug<THE_OTHER_TAG> & theOtherTag) {
                 Connector::dependsOn(*this, theOtherTag);
             }
             template <class THE_OTHER_TAG, class THE_FACADE>
             void dependsOn(PropertyPlug<THE_OTHER_TAG,THE_FACADE> & theOtherTag) {
                 Connector::dependsOn(*this, theOtherTag);
             }
             template <class THE_OTHER_TAG>
             void noLongerDependsOn(AttributePlug<THE_OTHER_TAG> & theOtherTag) {
                 Connector::noLongerDependsOn(*this, theOtherTag);
             }
             template <class THE_OTHER_TAG>
             bool alreadyDependsOn(AttributePlug<THE_OTHER_TAG> & theOtherTag) {
                 return Connector::alreadyDependsOn(*this, theOtherTag);
             }
             void dependsOn(ValuePtr theValue) {
                 AttributePlug<TAG>::_myAttribute->nodeValueWrapperPtr()->registerPrecursor(theValue);
             }
             void noLongerDependsOn(ValuePtr theValue) {
                 AttributePlug<TAG>::_myAttribute->nodeValueWrapperPtr()->unregisterPrecursor(theValue);
             }
             bool alreadyDependsOn(ValuePtr theValue) {
                 return AttributePlug<TAG>::_myAttribute->nodeValueWrapperPtr()->hasPrecursor(theValue);
             }
    };

    template<class FACADE>
    class DynamicAttributeMapper {
        typedef void (*UpdateFunc)(FACADE*, Node&);
        typedef std::map<std::string, UpdateFunc> UpdateFunctionMap;
    public:
        virtual ~DynamicAttributeMapper() {};
        static void setUpdateFunction(const std::string & theName, UpdateFunc theUpdateFunction) {
            _myUpdateFunctions[theName] = theUpdateFunction;
        }
        void updateAttribute(const std::string & theName, NodePtr & theNode) {
            typename UpdateFunctionMap::const_iterator myFunc = _myUpdateFunctions.find(theName);
            if (myFunc != _myUpdateFunctions.end()) {
                UpdateFunc myUpdateFunc = myFunc->second;
                (*myUpdateFunc)(dynamic_cast<FACADE*>(this), *theNode);
            }
        }
    private:
        static UpdateFunctionMap _myUpdateFunctions;
    };
    template<class FACADE>
    typename DynamicAttributeMapper<FACADE>::UpdateFunctionMap DynamicAttributeMapper<FACADE>::_myUpdateFunctions;

    template<class TAG, class FACADE>
    class DynamicAttributePlug : public FacadeAttributePlug<TAG>,
        public virtual DynamicAttributeMapper<FACADE>
    {
    protected:
        typedef bool(FACADE::*Getter)(typename TAG::TYPE&) const;
        typedef typename TAG::TYPE VALUE;
        typedef typename ValueWrapper<VALUE>::Type WRAPPER;

        DynamicAttributePlug(FACADE * theFacade, Getter theGetter) :
        FacadeAttributePlug<TAG>(theFacade), _myGetter(theGetter)
        {
            FACADE::setUpdateFunction(TAG::getName(), &DynamicAttributePlug::updateNodeValue);
        }

        static void updateNodeValue(FACADE * theFacade, Node & theAttributeNode) {

            const VALUE & theValue
                = ValueHelper<VALUE, WRAPPER>::getValue(theAttributeNode.nodeValueWrapperPtr());

            //const typename TAG::TYPE & theValue = theAttributeNode.Node::nodeValueRef<typename TAG::TYPE>();

            if (theFacade->DynamicAttributePlug<TAG,FACADE>::updateBeforeRead(const_cast<typename TAG::TYPE&>(theValue))) {
                theAttributeNode.setVersion(theAttributeNode.documentVersion());
            }
        }

        virtual bool updateBeforeRead(typename TAG::TYPE & theValue) const {
            // to update the value before returning, we have to
            // cast away all the const'ness
            if ((*dynamic_cast<const FACADE*>(this).*_myGetter)(theValue)) {
                return true;
            }
            return false;
        }
    private:
#       if defined(_MSC_VER)
#           pragma warning (push,1)
#       endif //defined(_MSC_VER)
        Getter _myGetter;
#       if defined(_MSC_VER)
#          pragma warning (pop)
#       endif //defined(_MSC_VER)
    };

    /* @} */
}

#endif
