//============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//============================================================================

// foobar

#include "uCCP.h"
#include "CRC8.h"
#include "SerialDevice.h"

#include <asl/string_functions.h>
#include <asl/Dashboard.h> // needed for the statistic ascii table thingy

#include <string>

using namespace std;

namespace asl {

    uCCP::uCCP(SerialDevice * theTTY) :
        _myTransportLayer(theTTY),
        _myDroppedBytes(0),
        _myCRCErrors(0),
        _myCorruptPackets(0),
        _myPacketsSend(0),
        _myPacketsReceived(0),
        _myStartToken(0xffU),
        _myEndToken(0xfeU),
        _myPayloadSize(UINT_MAX),
        _myErrorChecking(CRC8_CHECKING)
    {}

    void
    uCCP::recive() {
        if (!_myTransportLayer) {
            throw uCCPException("Serial port is not initialized (null pointer).", PLUS_FILE_LINE);
        }

        const unsigned int BUFFER_SIZE = 1024;
        unsigned char myBuffer[BUFFER_SIZE];
        size_t myReadBytes(BUFFER_SIZE);

        do {
            _myTransportLayer->read((char*)myBuffer, myReadBytes);

            for (unsigned i = 0; i < myReadBytes; ++i) {
                if (_myCurrentFrame.empty()) {
                    // check for a start token
                    if (myBuffer[i] == _myStartToken) {
                        _myCurrentFrame.push_back(myBuffer[i]);
                    } else {
                        _myDroppedBytes++;
                    }
                } else {
                    _myCurrentFrame.push_back(myBuffer[i]);
                    if (myBuffer[i] == _myEndToken) {
                        parseFrame(_myCurrentFrame);
                        _myCurrentFrame.clear();
                    }
                }
            }
        } while (myReadBytes == BUFFER_SIZE);
    }

    void
    uCCP::send(const std::string & thePacket) {
        if (!_myTransportLayer) {
            throw uCCPException("Serial port is not initialized (null pointer).", PLUS_FILE_LINE);
        }

        if (_myPayloadSize == UINT_MAX) {
            if (thePacket.size() > 255) {
                throw uCCPException(string("Can not send ") + as_string(thePacket.size()) +
                                    " bytes. uCCP can only handle up to 255 bytes.", PLUS_FILE_LINE);
            }
        } else {
            if (thePacket.size() != _myPayloadSize) {
                throw uCCPException(string("Can not send ") + as_string(thePacket.size()) +
                                    " bytes. uCCP was set to fixed payload size of: " + as_string(_myPayloadSize),
                                    PLUS_FILE_LINE);
            }
        }

        Frame myFrame;

        // (1) Size field
        if (_myPayloadSize == UINT_MAX) {
            myFrame.push_back((unsigned char)(thePacket.size()));
        }

        // (2) Payload
        for (unsigned i = 0; i < thePacket.size(); ++i) {
            myFrame.push_back((unsigned char)(thePacket[i]));
        }

        // (3) CRC / Checksum
        if (_myErrorChecking == CRC8_CHECKING) {
            myFrame.push_back(crc8i(char(0xff), myFrame.begin(),unsigned( myFrame.size() )));
        } else if (_myErrorChecking == CHECKSUM_CHECKING) {
            unsigned char myChecksum = checksum(myFrame.begin(), myFrame.size());
            myFrame.push_back(myChecksum & 0xf0U);
            myFrame.push_back(myChecksum & 0x0fU);
        }

        // (4) Escape and copy to block

        // (5) Start Token
        Block myBlock;
        myBlock.appendUnsigned8(_myStartToken);

        for (Frame::iterator myIt = myFrame.begin(); myIt != myFrame.end(); myIt++) {
            appendEscaped(myBlock, *myIt);
        }

        // (6) End token
        myBlock.appendUnsigned8(_myEndToken);

        _myTransportLayer->write((char*)myBlock.begin(), myBlock.size());
        _myPacketsSend++;
    }

    bool
    uCCP::parseFrame(Frame & theData) {
        // (1) Check for correct framing and removes frame
        if (!parseFrameing(theData)) {
            return false;
        }

        // (2) Remove escaping
        unescape(theData);

        // (3) Check for crc/checksum errors and remove error checking bytes
        if (!parseErrorChecking(theData)) {
            return false;
        }

        // (3) Check for correct size and remove size bytes
        if (!parseSize(theData)) {
            return false;
        }

        // (6) Copy the Payload
        string myPacket;
        for (Frame::iterator myIt = theData.begin(); myIt != theData.end(); myIt++) {
            myPacket.append(1, *myIt);
        }
        _myPendingPackets.push(myPacket);
        _myPacketsReceived++;
        return true;
    }

    bool
    uCCP::parseFrameing(Frame & theFrame) {
        if ( _myCurrentFrame.front() != _myStartToken || _myCurrentFrame.back() != _myEndToken) {
            _myCorruptPackets++;
            return false;
        }

        // Remove framing
        _myCurrentFrame.pop_front();
        _myCurrentFrame.pop_back();
        return true;
    }

    bool
    uCCP::parseErrorChecking(Frame & theData) {
        switch (_myErrorChecking) {
            case CRC8_CHECKING:
                // the crc of the size byte plus the payload plus the crc in the package must
                // be zero
                if (crc8i(0xffU, theData.begin(), theData.size()) != 0) {
                    _myCRCErrors++;
                    return false;
                }
                theData.pop_back();
                break;
            case CHECKSUM_CHECKING:
                if (checksum(theData.begin(), theData.size()) != 0) {
                    _myCRCErrors++;
                    return false;
                }
                theData.pop_back();
                theData.pop_back();
                break;
            default:
                return true;

        }

        return true;
    }

    bool
    uCCP::parseSize(Frame & theData) {
        unsigned myPayloadSize = _myPayloadSize;
        if (myPayloadSize == UINT_MAX) {
            myPayloadSize = theData.front();
            _myCurrentFrame.pop_front();
        }

        if (theData.size() != myPayloadSize) {
            _myCorruptPackets++;
            return false;
        }

        return true;
    }

    void
    uCCP::unescape(Frame & theFrame) {
        Frame::iterator myIt = theFrame.begin();
        for (; myIt != theFrame.end(); myIt++) {
            if ((*myIt) == _myEndToken) {
                // remove the escape char. myIt points to the escaped char afterwards
                myIt = theFrame.erase(myIt);
                *myIt += _myEndToken;
            }
        }
    }

    void
    uCCP::appendEscaped(asl::Block & theBuffer, unsigned char theData) {
        if (theData == _myEndToken || theData == _myStartToken) {
            theBuffer.appendUnsigned8((unsigned char)(_myEndToken));
            theBuffer.appendUnsigned8(theData - _myEndToken);
        } else {
            theBuffer.appendUnsigned8(theData);
        }
    }

    std::string
    uCCP::popPacket() {
        string myPacket = _myPendingPackets.front();
        _myPendingPackets.pop();
        return myPacket;
    }

    unsigned
    uCCP::pendingPackets() const {
        return _myPendingPackets.size();
    }

    void
    uCCP::setFixedPayloadSize(unsigned theSize) {
        if (theSize == 0) {
            throw uCCPException("Payload size of zero bytes is illegal.", PLUS_FILE_LINE);
        }

        if (theSize != UINT_MAX && theSize > 255) {
            throw uCCPException(string("Illegal payload size of ") + asl::as_string(theSize) +
                        " bytes. uCCP can only handle up to 255 bytes.", PLUS_FILE_LINE);
        }

        _myPayloadSize = theSize;
    }

    unsigned
    uCCP::getDroppedBytes() const {
        return _myDroppedBytes;
    }

    unsigned
    uCCP::getCRCErrors() const {
        return _myCRCErrors;
    }

    unsigned
    uCCP::getCorruptPackets() const {
        return _myCorruptPackets;
    }

    unsigned
    uCCP::getPacketsSend() const {
        return _myPacketsSend;
    }

    unsigned
    uCCP::getPacketsReceived() const {
        return _myPacketsReceived;
    }

    std::ostream &
    uCCP::printStatistic(std::ostream & os) const {
        if (!_myTransportLayer) {
            throw uCCPException("Serial port is not initialized (null pointer).", PLUS_FILE_LINE);
        }

        Table myTable;
        myTable.addColumn("name", string("uCCP on ") + _myTransportLayer->getDeviceName(),
                          Table::JUSTIFIED_LEFT, Table::JUSTIFIED_LEFT);
        myTable.addColumn("value", "", '.', Table::JUSTIFIED_MIDDLE, 4);

        myTable.addRow();
        myTable.setField("name", "packets send");
        myTable.setField("value", asl::as_string(getPacketsSend()));

        myTable.addRow();
        myTable.setField("name", "packets recived");
        myTable.setField("value", asl::as_string(getPacketsReceived()));

        myTable.addRow();
        myTable.setField("name", "packets pending");
        myTable.setField("value", asl::as_string(pendingPackets()));

        myTable.addRow();
        myTable.setField("name", "corrupt packets");
        myTable.setField("value", asl::as_string(getCorruptPackets()));

        myTable.addRow();
        myTable.setField("name", "crc/checksum errors");
        myTable.setField("value", asl::as_string(getCRCErrors()));

        myTable.addRow();
        myTable.setField("name", "bytes dropped");
        myTable.setField("value", asl::as_string(getDroppedBytes()));

        return myTable.print(os);
    }

    std::ostream &
    uCCP::printPacketFormat(std::ostream & os) const {
        Table myTable;

        // Table header
        myTable.addColumn("start",   "Start Token", Table::JUSTIFIED_MIDDLE);

        if (_myPayloadSize == UINT_MAX) {
            myTable.addColumn("length",  "Length", Table::JUSTIFIED_MIDDLE);
        }

        myTable.addColumn("payload", "Payload", Table::JUSTIFIED_MIDDLE);

        if (_myErrorChecking == CRC8_CHECKING) {
            myTable.addColumn("check", "CRC", Table::JUSTIFIED_MIDDLE);
        } else if (_myErrorChecking == CHECKSUM_CHECKING) {
            myTable.addColumn("check",   "Checksum", Table::JUSTIFIED_MIDDLE);
        }

        myTable.addColumn("end", "End Token", Table::JUSTIFIED_MIDDLE);

        // Size row
        myTable.addRow();
        myTable.setField("start", " 1 Byte ");
        if (_myPayloadSize == UINT_MAX) {
            myTable.setField("length", " 1 Byte ");
        }
        if (_myPayloadSize == UINT_MAX) {
            myTable.setField("payload", " 1 - 255 Bytes ");
        } else {
            myTable.setField("payload", asl::as_string(_myPayloadSize));
        }
        if (_myErrorChecking == CRC8_CHECKING) {
            myTable.setField("check", " 1 or 2 Bytes ");
        } else if (_myErrorChecking == CHECKSUM_CHECKING) {
            myTable.setField("check", " 2 Bytes ");
        }
        myTable.setField("end", " 1 Byte ");

        // Byte pattern row
        myTable.addRow();
        myTable.addRow();
        myTable.setField("start", asl::as_string((unsigned)_myStartToken));
        if (_myPayloadSize == UINT_MAX) {
            myTable.setField("length", "$XX");
        }
        if (_myPayloadSize == UINT_MAX) {
            myTable.setField("payload", "$XX ... $XX");
        } else {
            string myPayloadString("$XX ", _myPayloadSize);
            myTable.setField("payload", string("$XX (") + asl::as_string(_myPayloadSize) + ")");
        }
        if (_myErrorChecking != NO_CHECKING) {
            myTable.setField("check", "$XX $XX");
        }
        myTable.setField("end", asl::as_string((unsigned)_myEndToken));

        return myTable.print(os);
    }
}
