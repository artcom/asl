/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Value.cpp,v $
//
//   $Revision: 1.7 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Value.h"
#include <asl/Logger.h>
#include <asl/console_functions.h>
#include <typeinfo>


using namespace std;
using namespace dom;
using namespace asl;

#define DB(x) // x
#define DB2(x) // x

namespace dom {

    std::string filteredName(const std::string & theName) {
        std::string myResult = theName;
        asl::findAndReplace(myResult, "class ", "");
        asl::findAndReplace(myResult, "struct ", "");
        asl::findAndReplace(myResult, "dom::", "");
        asl::findAndReplace(myResult, "asl::", "");
        asl::findAndReplace(myResult, "MakeAccessibleVector,", "");
        return myResult;
    }

    Field::~Field() {
        clearPrecursorFields();
        clearDependendFields();
    }

    void
    Field::transferDependenciesFrom(Field & theField) {
        DB(AC_TRACE << "transferDependenciesFrom() to "<<filteredName(typeid(*this).name())<< "@"<<(void*)this <<
            " from "<<filteredName(typeid(theField).name())<< "@"<<(void*)&theField);

        clearPrecursorFields();

        for (int i = 0; i < theField._myPrecursorFields.size(); ++i)  {
            FieldPtr myPrecursorField = theField._myPrecursorFields[i];
            if (myPrecursorField) {
                myPrecursorField->registerDependend(_mySelf.lock());
                myPrecursorField->eraseDependend(theField._mySelf);
            }
        }
        theField._myPrecursorFields.resize(0);

        clearDependendFields();

        for (int i = 0; i < theField._myDependendFields.size(); ++i)  {
            FieldPtr myDependantField = theField._myDependendFields[i].lock();
            if (myDependantField) {
                myDependantField->registerPrecursor(_mySelf.lock());
                myDependantField->erasePrecursor(theField._mySelf.lock());
            }
        }
        theField._myDependendFields.resize(0); 

        _myCalculator = theField._myCalculator;
        _myConnector = theField._myConnector;
        theField._myCalculator = asl::Ptr<CallBackBase, ThreadingModel>(0);
        theField._myConnector = asl::Ptr<CallBackBase, ThreadingModel>(0);
        _hasOutdatedDependencies = theField._hasOutdatedDependencies;
        _isRecalculating = theField._isRecalculating;
        onSetValue();
    }

    void 
    Field::clearPrecursorFields() {
        if (_myCalculator || _myPrecursorFields.size()) {
            DB(AC_TRACE << "clearPrecursorFields() on "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
            _myCalculator = asl::Ptr<CallBackBase, ThreadingModel>(0);

            for (int i = 0; i < _myPrecursorFields.size(); ++i)  {
                FieldPtr myPrecursorField = _myPrecursorFields[i];
                if (myPrecursorField) {
                    myPrecursorField->eraseDependend(_mySelf);
                }
            }
            _myPrecursorFields.resize(0);
        }
    }

    void 
    Field::clearDependendFields() {
        if (_myDependendFields.size()) {
            DB(AC_TRACE << "clearDependendFields() on "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
            for (int i = 0; i < _myDependendFields.size(); ++i)  {
                FieldPtr myDependantField = _myDependendFields[i].lock();
                if (myDependantField) {
                    myDependantField->erasePrecursor(_mySelf.lock());
                }
            }
            _myDependendFields.resize(0);
        }
    }

    bool
    Field::hasPrecursor(FieldPtr theField) {
        return findPrecursor(theField) < _myPrecursorFields.size();
    }

    bool
    Field::hasDependend(FieldWeakPtr theField) {
        return findDependend(theField) < _myDependendFields.size();
    }

    void
    Field::registerPrecursor(FieldPtr theField) {
        if (!theField) {
            AC_ERROR << "Field::registerPrecursor: theField is null, throwing";
            throw InvalidNullPointerPassed(JUST_FILE_LINE);
        }
        DB(AC_TRACE << "registerPrecursor() "<<filteredName(typeid(*theField).name())<< "@"
            <<(void*)theField.getNativePtr()<<" at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        if (hasPrecursor(theField)) {
            AC_ERROR << "Field::registerPrecursor: theField "<<(void*)theField.getNativePtr() << " is already registered, ignored";
        } else {
            _myPrecursorFields.push_back(theField);
            theField->registerDependend(_mySelf.lock());
            _isDirty = true;
            setDependentsDirty();
        }
    }

    void 
    Field::unregisterPrecursor(FieldPtr theField) {
         if (!theField) {
            AC_ERROR << "Field::unregisterPrecursor: theField is null, ignored";
            throw InvalidNullPointerPassed(JUST_FILE_LINE);
        }
        DB(AC_TRACE << "unregisterPrecursor() "<<filteredName(typeid(*theField).name())<< "@"
            <<(void*)theField.getNativePtr()<<" at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        unsigned i = findPrecursor(theField);
        if (i < _myPrecursorFields.size()) {
            _myPrecursorFields[i]->eraseDependend(_mySelf);
            _myPrecursorFields.erase(_myPrecursorFields.begin() + i);
            return;
        }
        AC_ERROR << "unregisterPrecursor() "<<filteredName(typeid(*theField).name())<< "@"<<(void*)theField.getNativePtr()<<" not found at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
    }

    void 
    Field::erasePrecursor(FieldPtr theField) {
        if (!theField) {
            AC_ERROR << "Field::erasePrecursor: theField is null, ignored";
            throw InvalidNullPointerPassed(JUST_FILE_LINE);
        }
        DB(AC_TRACE << "erasePrecursor() "<<filteredName(typeid(*theField).name())<< "@"
            <<(void*)theField.getNativePtr()<<" at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        unsigned i = findPrecursor(theField);
        if (i < _myPrecursorFields.size()) {
            _myPrecursorFields.erase(_myPrecursorFields.begin() + i);
            return;
        }
        AC_ERROR << "erasePrecursor() "<<filteredName(typeid(*theField).name())<< "@"<<(void*)theField.getNativePtr()<<" not found at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
        return;
    }

    void 
    Field::unregisterDependend(FieldWeakPtr theField) {
         if (!theField) {
            AC_ERROR << "Field::unregisterDependend: theField is null, ignored";
            throw InvalidNullPointerPassed(JUST_FILE_LINE);
        }
        DB(AC_TRACE << "unregisterDependend() "<<filteredName(typeid(*theField.lock()).name())<< "@"
            <<(void*)theField.lock().getNativePtr()<<" at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        unsigned i = findDependend(theField.lock());
        if (i < _myDependendFields.size()) {
            _myDependendFields[i].lock()->erasePrecursor(_mySelf.lock());
            _myDependendFields.erase(_myDependendFields.begin() + i);
            return;
        }
        AC_ERROR << "unregisterDependend() "<<filteredName(typeid(*theField.lock()).name())<< "@"<<(void*)theField.lock().getNativePtr()<<" not found at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
    }

    void 
    Field::eraseDependend(FieldWeakPtr theField) {
        DB(
        if (theField) {
            AC_TRACE << "eraseDependend() "<<filteredName(typeid(*theField.lock()).name())<< "@"<<(void*)theField.lock().getNativePtr()<<" at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
        } else {
            AC_TRACE << "eraseDependend() of zero weak ptr at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
        })
        unsigned i = findDependend(theField);
        if (i < _myDependendFields.size()) {
            _myDependendFields.erase(_myDependendFields.begin() + i);
            return;
        }
        AC_ERROR << "eraseDependend() "<<filteredName(typeid(*theField.lock()).name())<< "@"<<(void*)theField.lock().getNativePtr()<<" not found at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
    }

    unsigned 
    Field::findDependend(FieldWeakPtr theField) const {
        for (unsigned i = 0; i < _myDependendFields.size(); ++i)  {
            if (_myDependendFields[i] == theField) {
                return i;
            }
        }
        return _myDependendFields.size();
    }
    unsigned 
    Field::findPrecursor(FieldPtr theField) const {
        for (unsigned i = 0; i < _myPrecursorFields.size(); ++i)  {
            if (_myPrecursorFields[i] == theField) {
                return i;
            }
        }
        return _myPrecursorFields.size();
    }

    void 
    Field::registerDependend(FieldPtr theField) {
        if (theField) {
            DB(AC_TRACE << "registerDependend() "<<filteredName(typeid(*theField).name())<< "@"
                <<(void*)theField.getNativePtr()<<" at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
            if (hasDependend(theField)) {
                AC_WARNING << "registerDependend():the field "<<filteredName(typeid(*theField).name())
                    << "@"<<(void*)theField.getNativePtr()<<" is already registered at "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
            } else {
                _myDependendFields.push_back(theField);
            }
        } else {
            AC_ERROR << "Field::registerDependend: theField is null";
            throw InvalidNullPointerPassed(JUST_FILE_LINE);
        }
    }
    
    void 
    Field::printDependendGraph(const std::string & myPrefix) const {
        if (myPrefix.size() == 0) {
            cerr << "Dependend fields of ";
        }
        std::string myClassString = filteredName(typeid(*this).name()) + " @ " + as_string((void*)this);
        if (_isDirty) {
            cerr << myPrefix <<  "["<< myClassString <<"]" << TTYRED << " (value dirty)" << ENDCOLOR;
        } else {
            cerr << myPrefix <<  "["<< myClassString <<"]" << TTYGREEN << " (value clean)" << ENDCOLOR;
        }
        if (_hasOutdatedDependencies) {
            cerr << TTYRED << " (dep. outdated)" << ENDCOLOR << endl;
        } else {
            cerr << TTYGREEN << " (dep. up-to-date)" << ENDCOLOR << endl;
        }
       for (int i = 0; i < _myDependendFields.size(); ++i)  {
        if (myPrefix.size() == 0) {
            _myDependendFields[i].lock()->printDependendGraph(myPrefix+"|-->");
            } else {
            _myDependendFields[i].lock()->printDependendGraph("    "+myPrefix);
            }
        }
    }

    void 
    Field::printPrecursorGraph(const std::string & myPrefix) const {
        if (myPrefix.size() == 0) {
            cerr << "Precursor fields of ";
        }
        std::string myClassString = filteredName(typeid(*this).name()) + " @ " + as_string((void*)this);
        if (_isDirty) {
            cerr << myPrefix <<  "["<< myClassString <<"]" << TTYRED << " (value dirty)" << ENDCOLOR;
        } else {
            cerr << myPrefix <<  "["<< myClassString <<"]" << TTYGREEN << " (value clean)" << ENDCOLOR;
        }
        if (_hasOutdatedDependencies) {
            cerr << TTYRED << " (dep. outdated)" << ENDCOLOR << endl;
        } else {
            cerr << TTYGREEN << " (dep. up-to-date)" << ENDCOLOR << endl;
        }

        for (int i = 0; i < _myPrecursorFields.size(); ++i)  {
            if (myPrefix.size() == 0) {
                _myPrecursorFields[i]->printPrecursorGraph(myPrefix+"|<--");
            } else {
                _myPrecursorFields[i]->printPrecursorGraph("    "+myPrefix);
            }
        }
    }

    void 
    Field::onSetValue() {
        ensureDependencies();
        if (_myCalculator && !_isRecalculating) {
            AC_WARNING << "onSetValue: you should not set dependent values manually, only in recalculate callback";
        }
        DB(
        if (_isDirty) {
            AC_TRACE << "onSetValue() of dirty (now clean) "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
        } else {
            AC_TRACE << "onSetValue() of clean (now also clean) "<<filteredName(typeid(*this).name())<< "@"<<(void*)this;
        })
        _isDirty = false;
        setDependentsDirty();
    }
    void
    Field::ensureDependencies() const {
        DB2(AC_TRACE << "ensureDependencies() of "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        if (_hasOutdatedDependencies && _myConnector) {
            const_cast<Field*>(this)->reconnect();
        }
    }


    void 
    Field::onGetValue() const {
        ensureDependencies();
        if (_isDirty) {
            _isRecalculating = true;
            const_cast<Field*>(this)->recalculate();
            _isRecalculating = false;
            _isDirty = false;
        }
    }

    void 
    Field::setDirty() {
        DB(AC_TRACE << "setDirty() of "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        ensureDependencies();
        if (!_isDirty) {
            _isDirty = true;
            setDependentsDirty();
        }
    }
    void
    Field::setDependentsDirty() {
        DB(AC_TRACE << "setDependentsDirty() of "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        for (int i = 0; i < _myDependendFields.size(); ++i)  {
            FieldPtr myDependantField = _myDependendFields[i].lock();
            if (myDependantField) {
                myDependantField->setDirty();
            } else {
                // remove from own list
                AC_WARNING << "setDependentsDirty: should not have null dependent field";
            }
        }
    }

    void 
    Field::setReconnectFunction(asl::Ptr<CallBackBase, ThreadingModel> theCallBack) {
        DB(AC_TRACE << "setReconnectFunction() on "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        _myConnector = theCallBack;
        _hasOutdatedDependencies = true;
    }
    void 
    Field::reconnect() {
        DB(AC_TRACE << "reconnect() of "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
        if (_myConnector) {
            DB(AC_TRACE << "reconnect() callback() "<<filteredName(typeid(*this).name())<< "@"<<(void*)this);
            _myConnector->callback();
        }
    }
}

