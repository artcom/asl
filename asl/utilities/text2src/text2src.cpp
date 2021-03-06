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
// Description: command line utility to place long text in source code files
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/Arguments.h>

#include <iostream>
#include <string>

using namespace std;
using namespace asl;

int main(int argc, char *argv[]) {

    Arguments::AllowedOptionWithDocumentation myOptions[] = {
        {"--varname", "name", " variable name, replaces every occurence of '@@@@' in prolog, concatenator or prolog"},
        {"--namespace", "name", " namespace name, replaces every occurence of '||||' in prolog, concatenator or prolog"},
        {"--newline", "pattern", " replace this pattern (default: |<- ) in prolog, concatenator or prolog with a newline"},
        {"--include", "name", " additional header to be included"},
        {"--prolog", "text", " text to be inserted at the beginning of the file"},
        {"--concatenator", "text", " text to be inserted every <maxlines> lines"},
        {"--epilog", "text", " text to be inserted at the end of the file"},
        {"--inputfile", "name", "read text from this path"},
        {"--outputfile", "name","write result to this path (default writes to stdout)"},
        {"--maxlines", "count", "split input every <count> lines with concatenator"},
        {"", "[text]", "text to be put into commandline"},
        {"", ""}
    };
    std::string newLinePattern = "|<-";
    std::string myVarName("MyVar");
    std::string myNamespace("MyNamespace");
    std::string myInclude("");

    // generate prolog
    ostringstream myPrologStream;
    myPrologStream << "#include <string>" << endl;
    myPrologStream << "namespace |||| {"<< endl;
    myPrologStream << "const std::string @@@@ = std::string("<< endl;
    std::string myProlog = myPrologStream.str();

    std::string myConcatenator(") + std::string(");
    std::string myEpilog(");|<-}");
    unsigned int myMaxLines = 100;

    ostringstream myExample;
    myExample << "Defaults:" << endl;
    myExample << " text2cpp --prolog \"" << myProlog << '"' << endl;
    myExample << "         --concatenator \"" << myConcatenator << '"' << endl;
    myExample << "         --epilog \""<< myEpilog << '"' << endl;
    myExample << "         --maxlines " << myMaxLines << endl;
    myExample << "         --newline \"" << newLinePattern << '"' << endl;
    myExample << "Examples: text2src --namespace y60 --variable ourY60xsd --inputfile Y60.xsd --outputfile Y60xsd.cpp" << endl;
    myExample << "          text2src --prolog \"const char * revision=\" --epilog=\";\"  --outputfile revision.cpp \"Rev.12345\"" << endl;
    myExample << "    The following line will produce a C++ program that outputs the text from myFile.txt to stdout:" << endl;
    myExample << "          text2src --inputfile myFile.txt --outputfile myFile.cpp --epilog \");}|<-#include <iostream>|<-int main() {std::cout << ||||::@@@@;}\"" << endl;

    std::string myInputFilename;
    std::string myOutputFilename;
    std::string myText;
    std::vector<std::string> myInputLines;

    try {
        Arguments myArguments;
        myArguments.addAllowedOptionsWithDocumentation(myOptions);
        myArguments.setShortDescription("text2src creates src files that puts long text into a variable");
        myArguments.setLongDescription(myExample.str());

        if (myArguments.parse(argc,argv)) {
            if (myArguments.getCount() && myArguments.haveOption("--inputfile")) {
                cerr << "### ERROR: text given both on commandline and with --inputfile option, please pick one" << myInputFilename << "'"<< endl;
                return 1;
            }
            if (myArguments.haveOption("--prolog")) {
                myProlog = myArguments.getOptionArgument("--prolog");
            }
            if (myArguments.haveOption("--concatenator")) {
                myConcatenator = myArguments.getOptionArgument("--concatenator");
            }
            if (myArguments.haveOption("--epilog")) {
                myEpilog = myArguments.getOptionArgument("--epilog");
            }
            if (myArguments.haveOption("--varname")) {
                myVarName = myArguments.getOptionArgument("--varname");
            }
            if (myArguments.haveOption("--include")) {
                myInclude = myArguments.getOptionArgument("--include");
            }
            findAndReplace(myProlog, "@@@@", myVarName.c_str());
            findAndReplace(myConcatenator, "@@@", myVarName.c_str());
            findAndReplace(myEpilog, "@@@@", myVarName.c_str());

            if (myArguments.haveOption("--namespace")) {
                myNamespace = myArguments.getOptionArgument("--namespace");
            }
            findAndReplace(myProlog, "||||", myNamespace.c_str());
            findAndReplace(myConcatenator, "||||", myNamespace.c_str());
            findAndReplace(myEpilog, "||||", myNamespace.c_str());

            if (myArguments.haveOption("--newline")) {
                newLinePattern = myArguments.getOptionArgument("--newline");
            }
            if (newLinePattern.size()) {
                ostringstream myNewLine;
                myNewLine << endl;
                findAndReplace(myProlog, newLinePattern.c_str(), myNewLine.str().c_str());
                findAndReplace(myConcatenator, newLinePattern.c_str(), myNewLine.str().c_str());
                findAndReplace(myEpilog, newLinePattern.c_str(), myNewLine.str().c_str());
            }

            string myFileText;
            if (myArguments.haveOption("--inputfile")) {
                myInputFilename = myArguments.getOptionArgument("--inputfile");
                if (!readFile(myInputFilename, myInputLines)) {
                    cerr << "### ERROR: could not read file '" << myInputFilename << "'" << endl;
                    return 1;
                }
            }
            if (myArguments.haveOption("--outputfile")) {
                myOutputFilename = myArguments.getOptionArgument("--outputfile");
            }
            if (myArguments.haveOption("--maxlines")) {
                myMaxLines =  asl::as<int>(myArguments.getOptionArgument("--maxlines"));
            }

            if (myArguments.getCount()>0) {
                for (int i = 0; i < myArguments.getCount(); ++i) {
                    myInputLines.push_back(myArguments.getArgument(i));
                }
            }

            std::ostringstream myOutput;

            myOutput << "/* --- Generated by text2src, do not modify --- */" << endl;

            if(myInclude.size() > 0) {
              myOutput <<
                "#include \"" << myInclude << "\"\n" << endl;
            }

            myOutput << myProlog;
            for (unsigned int myLine = 0; myLine < myInputLines.size();++myLine) {
                myOutput << '"';
                std::string myOutputLine = myInputLines[myLine];
                findAndReplace(myOutputLine, "\\", "\\\\");
                findAndReplace(myOutputLine, "\"", "\\\"");
                myOutput << myOutputLine;
#ifdef NO_LAST_NEWLINE
                if (myLine+1 < myInputLines.size()) {
#endif
                    myOutput << "\\n";
#ifdef NO_LAST_NEWLINE
                }
#endif
                myOutput << '"';
                myOutput << endl;
                if (myLine % myMaxLines == 0) {
                    myOutput << myConcatenator << endl;
                }
            }
            myOutput << myEpilog << endl;

            if (myOutputFilename.size()) {
                if (!writeFile(myOutputFilename, myOutput.str())) {
                    cerr << "### ERROR: could not write file '" << myInputFilename << "'"<< endl;
                    return 1;
                }
            } else {
                cout << myOutput.str();
            }
        }

    } catch (asl::Exception & ex) {
            cerr << "### ERROR: " << ex << endl;
        return -1;
    }
    return 0;
}

