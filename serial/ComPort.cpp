/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: ComPort.cpp,v $
//     $Author: thomas $
//   $Revision: 1.18 $
//
//=============================================================================

//own header
#include "ComPort.h"

#include <asl/base/Block.h>
#include <asl/base/Logger.h>

#include <asl/base/error_functions.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>

using namespace std;

#define DB(x) // x

namespace asl {

    ComPort::ComPort(const std::string & theDeviceName) :
        SerialDevice(theDeviceName),
        _myHandle(INVALID_HANDLE_VALUE)
    {
        AC_DEBUG << "ComPort::ComPort()";
    }

    ComPort::~ComPort() {
        AC_DEBUG << "ComPort::~ComPort()";

        if (isOpen()) {
            close();
        }
    }

    void
    ComPort::open(unsigned int theBaudRate, unsigned int theDataBits,
                  ParityMode theParityMode, unsigned int theStopBits,
                  bool theHWHandShakeFlag,
                  int theMinBytesPerRead, int theTimeout)
    {
        AC_DEBUG << "ComPort::open()";

        if(isOpen()) {
            throw SerialPortException("Failure in open: device allready open", PLUS_FILE_LINE);
        }

        AC_DEBUG << "Opening " << getDeviceName() << " with: "
                 << " baudrate: "  << theBaudRate
                 << " bits: "      << theDataBits
                 << " parity: "   << theParityMode
                 << " stopbits: "  << theStopBits
                 << " handshake: " << theHWHandShakeFlag << endl;

        // open our handle
        AC_TRACE << "Opening device";
        HANDLE myHandle = CreateFile(getDeviceName().c_str(),
                                     GENERIC_READ | GENERIC_WRITE,
                                     0,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL);
        if(myHandle == INVALID_HANDLE_VALUE) {
            handleSystemError("CreateFile", PLUS_FILE_LINE);
        }

        // get the comm properties for use during initialization
        //  XXX: not used right now but might be needed
        AC_TRACE << "Getting comm properties";
        COMMPROP myProperties;
        if(!GetCommProperties(myHandle, &myProperties)) {
            handleSystemError("GetCommProperties", PLUS_FILE_LINE);
        }

        // configure device control parameters
        //   XXX: verify if we set enough fields.
        //        there might be environmental state leaks in this.
        DCB myControlBlock;

        // initialize control block with current state
        AC_TRACE << "Getting comm state";
        if (!GetCommState(myHandle, &myControlBlock)) {
            handleSystemError("GetCommState", PLUS_FILE_LINE);
        }

        // common serial port parameters
        myControlBlock.BaudRate = theBaudRate;
        myControlBlock.ByteSize = static_cast<BYTE>(theDataBits);
        myControlBlock.Parity   = convertParity(theParityMode);
        myControlBlock.StopBits = convertStopBits(theStopBits);
        myControlBlock.fRtsControl = (theHWHandShakeFlag ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE);
        myControlBlock.fDtrControl = (theHWHandShakeFlag ? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_DISABLE);

        // make handle unusable when it errors
        // handle can be reset by reading comm errors
        myControlBlock.fAbortOnError = true;

        // apply control block settings
        AC_TRACE << "Setting comm state";
        if (!SetCommState(myHandle, &myControlBlock)) {
            handleSystemError("SetCommState", PLUS_FILE_LINE);
        }

        // configure device timing parameters
        COMMTIMEOUTS myTimeouts;
        if (theMinBytesPerRead == 0 && theTimeout == 0) {
            // nonblocking
            myTimeouts.ReadIntervalTimeout = MAXDWORD;
            myTimeouts.ReadTotalTimeoutMultiplier = 0;
            myTimeouts.ReadTotalTimeoutConstant = 0;
        } else if (theMinBytesPerRead > 0 && theTimeout == 0) {
            // fully blocking
            myTimeouts.ReadIntervalTimeout = 0;
            myTimeouts.ReadTotalTimeoutMultiplier = 0;
            myTimeouts.ReadTotalTimeoutConstant = 0;
        } else if (theMinBytesPerRead > 0 && theTimeout > 0) {
            // block until interframe timeout
            myTimeouts.ReadIntervalTimeout = theTimeout * 100;
            myTimeouts.ReadTotalTimeoutMultiplier = 0;
            myTimeouts.ReadTotalTimeoutConstant = 0;
        } else if (theMinBytesPerRead == 0 && theTimeout > 0) {
            // block until at least one byte arrives
            myTimeouts.ReadIntervalTimeout = MAXDWORD;
            myTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
            myTimeouts.ReadTotalTimeoutConstant = theTimeout * 100;
        }

        // TODO: Think about write timeouts
        myTimeouts.WriteTotalTimeoutConstant = 0;
        myTimeouts.WriteTotalTimeoutMultiplier = 0;

        // apply blocking configuration
        AC_TRACE << "Setting comm timeouts";
        if (!SetCommTimeouts(myHandle, &myTimeouts)) {
            handleSystemError("SetCommTimeouts", PLUS_FILE_LINE);
        }

        // set buffer size to maximum
        AC_TRACE << "Setting comm buffer sizes";
        if(!SetupComm(myHandle, 64*1024, 64*1024)) {
            handleSystemError("SetupComm", PLUS_FILE_LINE);
        }

        // finally, set our handle
        _myHandle = myHandle;

        isOpen(true);
    }

    void
    ComPort::close() {
        AC_DEBUG << "ComPort::close()";

        ensureDeviceOpen("close", PLUS_FILE_LINE);

        if(!CloseHandle(_myHandle)) {
            handleSystemError("CloseHandle", PLUS_FILE_LINE);
        }

        _myHandle = INVALID_HANDLE_VALUE;

        isOpen(false);
    }

    bool
    ComPort::read(char * theBuffer, size_t & theSize) {
        AC_TRACE << "ComPort::read()";

        ensureDeviceOpen("read", PLUS_FILE_LINE);
        checkCommError(PLUS_FILE_LINE);

        AC_TRACE << "Reading from device";
        size_t myReadBytes;
        if(!ReadFile(_myHandle, theBuffer, DWORD(theSize), (DWORD *) & myReadBytes, NULL)) {
            DWORD myError = GetLastError();
            if(myError != ERROR_OPERATION_ABORTED) {
                handleSystemError("ReadFile", PLUS_FILE_LINE, myError);
            } else {
                // we accept and ignore OPERATION_ABORTED, which is the win32 equivalent of EAGAIN
                // XXX: we might want to re-read, but handling that might be horror
                theSize = 0;
                return false;
            }
        }

        if (theSize == myReadBytes) {
            return true;
        } else {
            theSize = myReadBytes;
            return false;
        }
    }

    void
    ComPort::write(const char * theBuffer, size_t theSize) {
        AC_TRACE << "ComPort::write()";

        ensureDeviceOpen("write", PLUS_FILE_LINE);
        checkCommError(PLUS_FILE_LINE);

        AC_TRACE << "Writing to device";
        size_t myWrittenBytes;
        if(!WriteFile(_myHandle, theBuffer, theSize,  (DWORD *)& myWrittenBytes, NULL)) {
            DWORD myError = GetLastError();
            if(myError != ERROR_OPERATION_ABORTED) {
                handleSystemError("WriteFile", PLUS_FILE_LINE, myError);
            } else {
                // we accept and ignore OPERATION_ABORTED, which is the win32 equivalent of EAGAIN.
                // however, since we do not offer an incremental writing interface, we are bound to fail.
                myWrittenBytes = 0;
            }
        }

        // XXX: the following "error handling" is total utter bullshit and should be replaced with
        //      either queueing or incremental write support (returning a written-byte count).
        if (theSize != myWrittenBytes) {
            throw SerialPortException(string("Can not write ") + asl::as_string(theSize) +
                                   " bytes to device '" + getDeviceName() +
                                   "'. Only " + asl::as_string(myWrittenBytes) + " bytes got written.",
                                   PLUS_FILE_LINE);
        }
    }

    void 
    ComPort::flush() {
        ensureDeviceOpen("flush", PLUS_FILE_LINE);
        if (!PurgeComm( _myHandle, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
            handleSystemError("PurgeComm", PLUS_FILE_LINE);
        }
    }

    void
    ComPort::setStatusLine(unsigned theStatusMask) {
        ensureDeviceOpen("setStatusLine", PLUS_FILE_LINE);

        if (!EscapeCommFunction(_myHandle, (theStatusMask & DTR ? SETDTR : CLRDTR))) {
            throw SerialPortException(string("Unable to modify DTR on device '") +
                getDeviceName() + "'.", PLUS_FILE_LINE);
        }
        if (!EscapeCommFunction(_myHandle, (theStatusMask & RTS ? SETRTS : CLRRTS))) {
            throw SerialPortException(string("Unable to modify RTS on device '") +
                getDeviceName() + "'.", PLUS_FILE_LINE);
        }
    }

    unsigned
    ComPort::getStatusLine() {
        ensureDeviceOpen("getStatusLine", PLUS_FILE_LINE);

        DWORD dwModemStatus = 0;
        if (!GetCommModemStatus(_myHandle, &dwModemStatus)) {
            handleSystemError("GetCommModemStatus", PLUS_FILE_LINE);
        }

        unsigned myStatusMask = 0;
        if (dwModemStatus & MS_CTS_ON) {
            myStatusMask |= CTS;
        }
        if (dwModemStatus & MS_DSR_ON) {
            myStatusMask |= DSR;
        }
        if (dwModemStatus & MS_RING_ON) {
            myStatusMask |= RI;
        }
        if (dwModemStatus & MS_RLSD_ON) {
            myStatusMask |= CD;
        }

        return myStatusMask;
    }

    void
    ComPort::checkCommError(const std::string & theLocation) {
        AC_TRACE << "Clearing and handling comm errors";

        DWORD myCommErrors = 0;
        if(!ClearCommError(_myHandle, &myCommErrors, NULL)) {
            handleSystemError("ClearCommError", PLUS_FILE_LINE);
        }

        if(myCommErrors) {
            std::string myErrorString;
            commErrorsToString(myCommErrors, myErrorString);
            throw SerialPortException(
                "The following communication errors have occured on serial port " +
                getDeviceName() + ":" + myErrorString, theLocation);
        }
    }

    void
    ComPort::commErrorsToString(DWORD theErrors, std::string & theString) {
        if(theErrors & CE_OVERRUN) {
            theString += " OVERRUN";
        }
        if(theErrors & CE_FRAME) {
            theString += " FRAMING";
        }
        if(theErrors & CE_BREAK) {
            theString += " BREAK";
        }
        if(theErrors & CE_RXOVER) {
            theString += " RX_OVERRUN";
        }
        if(theErrors & CE_RXPARITY) {
            theString += " RX_PARITY";
        }
        if(theErrors & CE_IOE) {
            theString += " IO_ERROR";
        }
        if(theErrors & CE_TXFULL) {
            theString += " TX_FULL";
        }
    }

    void
    ComPort::handleSystemError(const std::string& theSystemCall, const std::string & theLocationString, const DWORD theError) {
        if (theError) {
            LPVOID myMessageBuffer;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS, NULL, theError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR) & myMessageBuffer, 0, NULL);
            string myErrorString = theSystemCall + " on serial port " + getDeviceName() + " failed: ";
            myErrorString.append((LPTSTR) myMessageBuffer);
            LocalFree(myMessageBuffer);
            throw SerialPortException(myErrorString, theLocationString);
        }
    }

    void
    ComPort::ensureDeviceOpen(const std::string & theMethodCall, const std::string & theLocationString) {
        if(!isOpen()) {
            throw SerialPortException(theMethodCall + " on serial port " + getDeviceName() + " failed: Device not open", theLocationString);
        }
    }

    BYTE
    ComPort::convertParity(ParityMode theParity) {
        switch (theParity) {
            case NO_PARITY:
                return NOPARITY;
            case EVEN_PARITY:
                return EVENPARITY;
            case ODD_PARITY:
                return ODDPARITY;
            default:
                throw SerialPortException("Unknown parity mode.", PLUS_FILE_LINE);
        }
    }

    BYTE
    ComPort::convertStopBits(unsigned int theStopBits) {
        switch (theStopBits) {
            case 1:
                return ONESTOPBIT;
            case 2:
                return TWOSTOPBITS;
            default:
                throw SerialPortException(string("Can not handle ") + asl::as_string(theStopBits) +
                                        " stop bits.", PLUS_FILE_LINE);
        }
    }
}
