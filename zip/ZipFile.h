/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: ZipFile.h,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl_zipfile_included_
#define __asl_zipfile_included_

#include <asl/Stream.h>
#include <asl/Block.h>
#include <string>

namespace asl {

DEFINE_EXCEPTION(ZipFileException, asl::Exception);
    
class ZipFile {
    public:
        struct Entry {
            std::string filename;
            size_t size;
            size_t directory_pos;
            size_t file_index;
        };
        typedef std::vector<Entry> Directory; 
        ZipFile(const char * theInputFileName);
        virtual ~ZipFile();
        const Directory & getDirectory() const;
        Ptr<ReadableBlock> getFile(const Entry & theEntry);
        Ptr<ReadableBlock> getFile(int theFileIndex);
        Ptr<ReadableBlock> getFile(const std::string & theFilePath);
    private:
        ZipFile();
        ZipFile(const ZipFile &);
        ZipFile & operator=(const ZipFile &);
        void readDirectory();
        Directory _myDirectory;
        void * _myInputStream;
};

typedef Ptr<ZipFile> ZipFilePtr;

}

#endif
