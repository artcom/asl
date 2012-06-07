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

#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/Arguments.h>

#include <iostream>
#include <string>


using namespace std;  // automatically added!



using namespace asl;

bool isNameChar(char c) {
    return is_alpha(c) || is_digit(c) || is_underscore(c) || c == '.';
};


bool isQuoted(const string & myLine, string::size_type myPosInLine) {
    int myApostrophCount = 0;
    string::size_type myPos = myPosInLine;
    while ((myPos = myLine.rfind('"', myPos)) != string::npos) {
        //cerr << "myPos=" << myPos << endl;
        if (myPos > 0) {
            if ((myLine[myPos-1] == '"') || (myLine[myPos-1] == '\\')) {
                --myPos;
                --myPos;
                continue;
            }
            --myPos;
        } else {
            break;
        }
        ++myApostrophCount;
    }
    return myApostrophCount % 2 != 0;
}

bool isCppComment(const string & myLine, string::size_type myPosInLine) {
    if ((myPosInLine = myLine.rfind("//", myPosInLine)) != string::npos) {
        return !isQuoted(myLine, myPosInLine);
    }
    return false;
}

void getLine(const string & myFile, string::size_type myPosInFile, string & myLine, string::size_type & myPosInLine) {
    string::size_type myLineStart = myFile.rfind('\n',myPosInFile) + 1;
    string::size_type myLineEnd = myFile.find('\n',myPosInFile);
    myLine = myFile.substr(myLineStart, myLineEnd - myLineStart);
    myPosInLine = myPosInFile - myLineStart;
}

void getLine(const string & myFile, string::size_type myPosInFile, string::size_type myMatchSize, string & myLine, string::size_type & myPosInLine) {
    string::size_type myLineStart = myFile.rfind('\n',myPosInFile) + 1;
    string::size_type myLineEnd = myFile.find('\n',myPosInFile+myMatchSize);
    myLine = myFile.substr(myLineStart, myLineEnd - myLineStart);
    myPosInLine = myPosInFile - myLineStart;
}

string::size_type find_C_CommentToken(const string & myFile, string::size_type myPosInFile, const string & myToken) {
    string::size_type myTokenPos = myPosInFile;
    while ((myTokenPos = myFile.rfind(myToken, myTokenPos)) != string::npos) {
        string myLine;
        string::size_type myPosInLine;
        getLine(myFile, myTokenPos, myLine, myPosInLine);
        if (!isCppComment(myLine, myPosInLine) && !isQuoted(myLine, myPosInLine)) {
            // this is our normal positive exit
            break;
        };
        // try again
        --myTokenPos;
    }
    return myTokenPos;
}

bool isCComment(const string & myFile, string::size_type myPosInFile) {
    string::size_type myCommentStartPos = find_C_CommentToken(myFile, myPosInFile, "/*");
    if (myCommentStartPos != string::npos) {
        // we have found a real opening comment now, look for the latest closing comment
        string::size_type myCommentEndPos = find_C_CommentToken(myFile, myPosInFile, "*/");
        if (myCommentEndPos != string::npos) {
            return myCommentEndPos < myCommentStartPos;
        }
        return true;
    }
    return false;
}

int main(int argc, char *argv[]) {

    Arguments::AllowedOption myOptions[] = {
        {"-n", ""},
        {"-v", ""},
        {"-s", ""},
        {"-w", ""},
        {"-q", ""},
        {"-Q", ""},
        {"-c", ""},
        {"-C", ""},
        {"-2", ""},
        {"-p", "%d"},
        {"", ""}
    };

    try {
        Arguments myArguments;
        myArguments.addAllowedOptions(myOptions);

        if (myArguments.parse(argc,argv) && myArguments.getCount()>=3) {
            string searchForString = myArguments.getArgument(0);
            string replaceUntilString;
            unsigned firstFileArgument = 2;
            if (myArguments.haveOption("-2")) {
                replaceUntilString = myArguments.getArgument(1);
                ++firstFileArgument;
            }
            string::size_type matchOnlyPos = string::npos;
            if (myArguments.haveOption("-p")) {
                matchOnlyPos = asl::as<int>(myArguments.getOptionArgument("-p"));
            }


            string replaceWithString = myArguments.getArgument(firstFileArgument-1);
            for (int i = firstFileArgument; i < myArguments.getCount(); ++i) {
                string myFile;
                bool hasBeenModified = false;
                if (readFile(myArguments.getArgument(i), myFile)) {
                    string::size_type matchPos = 0;
                    string::size_type matchLen = searchForString.size();
                    while ((matchPos = myFile.find(searchForString, matchPos)) != string::npos) {
                        if (matchOnlyPos != string::npos) {
                            if (matchPos > matchOnlyPos) {
                               break;
                            }
                            if (matchPos < matchOnlyPos) {
                               continue;
                            }
                       }

                        string::size_type matchPos2 = 0;
                        if (myArguments.haveOption("-2")) {
                            matchPos2 = myFile.find(replaceUntilString, matchPos+searchForString.size());
                            if (matchPos2 == string::npos) {
                                break;
                            }
                            matchLen = matchPos2 - matchPos + replaceUntilString.size();
                        }

                        if (myArguments.haveOption("-w")) {
                            if (matchPos>0) {
                                if (isNameChar(myFile[matchPos-1]) ||
                                        isNameChar(myFile[matchPos+matchLen])) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                        }
                        if (!myArguments.haveOption("-s") ||
                                myArguments.haveOption("-q") ||
                                myArguments.haveOption("-Q") ||
                                myArguments.haveOption("-c") ||
                                myArguments.haveOption("-C"))
                        {

                            string myLine;
                            string::size_type myPosInLine;
                            getLine(myFile, matchPos, matchLen, myLine, myPosInLine);

                            if (myArguments.haveOption("-q")) {
                                if (isQuoted(myLine, myPosInLine)) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                            if (myArguments.haveOption("-Q")) {
                                if (!isQuoted(myLine, myPosInLine)) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                            if (myArguments.haveOption("-c")) {
                                if (isCppComment(myLine, myPosInLine) || isCComment(myFile, matchPos)) {
                                    ++matchPos;
                                    continue;
                                }
                            }
                            if (myArguments.haveOption("-C")) {
                                if (!isCppComment(myLine, myPosInLine) && !isCComment(myFile, matchPos)) {
                                    ++matchPos;
                                    continue;
                                }
                            }

                            if (myArguments.haveOption("-v")) {
                                cerr << "Match in file '" << myArguments.getArgument(i) << "', pos "
                                    << matchPos << ":" << endl;
                                cerr << "Replacing :" << myLine << endl;
                                cerr << "     with :";
                            }
                            myLine.replace(myPosInLine, matchLen, replaceWithString);
                            cout << myLine << endl;
                        }
                        hasBeenModified = true;
                        myFile.replace(matchPos, matchLen, replaceWithString);
                        matchPos+=replaceWithString.size();
                    };
                } else {
                    cerr << "### WARNING: could not read file '" << myArguments.getArgument(i) << endl;
                }
                if (!myArguments.haveOption("-n") && hasBeenModified) {
                    if (!writeFile(myArguments.getArgument(i), myFile)) {
                        cerr << "### WARNING: could not write file '" << myArguments.getArgument(i) << endl;
                    }
                }
            }
        } else {
            cerr << "usage: greplace [options] search-string [until-string] replace-string file [file2...fileN]" << endl;
            cerr << "    -s : enable silent mode " << endl;
            cerr << "    -v : enable verbose mode " << endl;
            cerr << "    -n : do not write back file " << endl;
            cerr << "    -w : match whole words only (no adjacent [A-F,a-f,0-9,_] char" << endl;
            cerr << "    -q : do not match strings between quotes ("")" << endl;
            cerr << "    -Q : do only match strings between quotes ("")" << endl;
            cerr << "    -c : do not match strings in c/c++ comments" << endl;
            cerr << "    -C : do only match strings in c/c++ comments" << endl;
            cerr << "    -2 : replace all characters from <search-string> up to (including) <until-string>" << endl;
            cerr << "    -p <charpos>: replace only match at specified character position" << endl;
            return -1;
        }
    }
    catch (asl::Exception & ex) {
        cerr << "### ERROR: " << ex << endl;
        return -1;
    }
    return 0;
}

