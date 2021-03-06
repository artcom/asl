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
//    $RCSfile: PackageManager.cpp,v $
//
//   $Revision: 1.11 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "PackageManager.h"

#include "DirectoryPackage.h"
#include "ZipPackage.h"
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>
#include <asl/base/Logger.h>
#include <vector>

namespace asl {

void
PackageManager::add(IPackagePtr thePackage) {
    for (PackageList::iterator it=_myPackages.begin(); it!=_myPackages.end(); ++it) {
        if ((*it)->getPath() == thePackage->getPath()) {
            AC_DEBUG << "Package '" << thePackage->getPath() << "' has already been added.";
            return;
        }
    }

    AC_DEBUG << "adding Package '" << thePackage->getPath() << "'";
    _myPackages.push_front(thePackage);
}

bool
PackageManager::remove(IPackagePtr thePackage) {
    for (PackageList::iterator it=_myPackages.begin(); it!=_myPackages.end(); ++it) {
        if ((*it)->getPath() == thePackage->getPath()) {
            _myPackages.erase(it);
            AC_DEBUG << "Package '" << thePackage->getPath() << "' has been removed.";
            return true;
        }
    }
    return false;
}

bool
PackageManager::remove(const std::string & thePackageName) {
    for (PackageList::iterator it=_myPackages.begin(); it!=_myPackages.end(); ++it) {
        if ((*it)->getPath() == thePackageName) {
            _myPackages.erase(it);
            AC_DEBUG << "Package '" << thePackageName << "' has been removed.";
            return true;
        }
    }
    return false;
}

void
PackageManager::add(const std::string & thePaths) {
    AC_TRACE << "adding '" << thePaths << "'";
    std::vector<std::string> myPathVector;
    splitPaths(thePaths, myPathVector);
    AC_DEBUG << "adding " << myPathVector.size() << " packages";
    for (int i = myPathVector.size() - 1; i >= 0; i--) {
        std::string normalizedPath = normalizeDirectory(myPathVector[i], true);

        if (!fileExists(normalizedPath)) {
            AC_WARNING << "PackageManager: Package '" << normalizedPath << "' does not exist";
            continue;
        }

        if (isDirectory(normalizedPath)) {
            AC_DEBUG << "adding Directory '" << normalizedPath << "'";
            add(IPackagePtr(new DirectoryPackage(normalizedPath)));
            continue;
        } else {
            ConstMappedBlock myBlock(normalizedPath);
            // check zip file signature
            if (myBlock[0] == 0x50 && myBlock[1] == 0x4b && myBlock[2] == 0x03 && myBlock[3] == 0x04) {
                AC_DEBUG << "adding Zip '" << normalizedPath << "'";
                add(IPackagePtr(new ZipPackage(normalizedPath)));
            } else {
                AC_WARNING << "PackageManager: can not open package '" << normalizedPath << "'"
                            << ", not a directory and file type unknown.";
                AC_WARNING << "Sig is " << myBlock[0] << myBlock[1] << myBlock[2] << myBlock[3];
            }
        }
    }
}

void
PackageManager::add(const PackageManager & theOther) {
    AC_TRACE << "add from package '" << theOther.getSearchPath() << "'";
    PackageList::const_iterator it = theOther._myPackages.begin();
    for (; it != theOther._myPackages.end(); ++it) {
        add(*it);
    }
}

IPackagePtr
PackageManager::findPackage(const std::string & theRelativePath,
                            const std::string & thePackage)
{
    AC_TRACE << "findPackage pkg='" << thePackage << "' path='" << theRelativePath << "'";
    std::string myPackage = normalizeDirectory(thePackage, true);
    AC_TRACE << "findPackage myPkg='" << myPackage << "'";
    for (PackageList::iterator iter = _myPackages.begin();
         iter != _myPackages.end(); ++iter)
    {
        if ((myPackage != "" && (*iter)->getPath() != myPackage)) {
            continue;
        }
        if (!((*iter)->findFile(theRelativePath).empty())) {
            return *iter;
        }
    }
    return IPackagePtr();
}

Ptr<ReadableStreamHandle>
PackageManager::readStream(const std::string & theRelativePath,
                         const std::string & thePackage)
{
    IPackagePtr myPackage = findPackage(theRelativePath, thePackage);
    if (myPackage) {
        return myPackage->getStream(theRelativePath);
    }
    return Ptr<ReadableStreamHandle>();
}

Ptr<ReadableBlockHandle>
PackageManager::readFile(const std::string & theRelativePath,
                         const std::string & thePackage,
                         bool theThreadLockedFlag)
{
    if (theThreadLockedFlag) {
        _myReadLock.lock();
    }
    Ptr<ReadableBlockHandle> myResult = Ptr<ReadableBlockHandle>();
    IPackagePtr myPackage = findPackage(theRelativePath, thePackage);
    if (myPackage) {
        myResult = myPackage->getFile(theRelativePath);
    }
    if (theThreadLockedFlag) {
        _myReadLock.unlock();
    }
    return myResult;
}

std::string
PackageManager::searchFile(const std::string & theRelativePath) const {
    AC_TRACE << "searchFile path='" << theRelativePath << "'";
    if (_myPackages.size() == 0) {
        AC_WARNING << "No packages to search!";
    }
    for (PackageList::const_iterator iter = _myPackages.begin();
         iter != _myPackages.end(); ++iter) {
        std::string myAbsolutePath = (*iter)->findFile(theRelativePath);
        if (!myAbsolutePath.empty()) {
            AC_DEBUG << "searchFile found file at path='" << myAbsolutePath << "'";
            return myAbsolutePath;
        }
    }
    if (fileExists(theRelativePath)) {
        AC_DEBUG << "searchFile found file at path='" << theRelativePath << "'";
        return theRelativePath;
    }
    return "";
}

IPackage::FileList
PackageManager::listPackageFiles(IPackagePtr thePackage,
                                 const std::string & theRelativePath,
                                 bool doRecursiveSearch)
{
    IPackage::FileList myFileList;
    if (thePackage && !(thePackage->findFile(theRelativePath).empty())) {
        myFileList = thePackage->getFileList(theRelativePath, doRecursiveSearch);
    }

    return myFileList;
}

IPackage::FileList
PackageManager::findFiles(const std::string & theRelativePath,
                          const std::string & thePackage,
                          bool doRecursiveSearch /*= false */)
{
    AC_TRACE << "findFiles pkg='" << thePackage << "' path='" << theRelativePath << "'";
    IPackage::FileList myFileList;
    if (!thePackage.empty()) {
        IPackagePtr myPackage = findPackage("", thePackage);
        myFileList = listPackageFiles(myPackage, theRelativePath, doRecursiveSearch);
    } else {
        for (PackageList::const_iterator iter = _myPackages.begin();
             iter != _myPackages.end(); ++iter) {
            IPackage::FileList myTmpFileList = listPackageFiles((*iter), theRelativePath, doRecursiveSearch);
            for (unsigned i = 0; i < myTmpFileList.size(); ++i) {
                myFileList.push_back(myTmpFileList[i]);
            }
        }
    }
    return myFileList;
}

std::string
PackageManager::getSearchPath() const {
    std::string mySearchPath;
    PackageList::const_iterator it = _myPackages.begin();
    for (; it != _myPackages.end(); ++it) {
        if ( ! mySearchPath.empty()) {
            mySearchPath += ";";
        }
        mySearchPath += (*it)->getPath();
    }
    return mySearchPath;
}

PackageList PackageManager::getPackageList() const {
    return _myPackages;
}

}
