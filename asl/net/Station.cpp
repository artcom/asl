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
// Description:
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//    recommendations: move this to its own component,
//                     check interfaces,
//                     remove CVS log
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "Station.h"

#include "net_functions.h"
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Time.h>
#include <asl/base/Logger.h>

#include <zlib.h>

#define DB0(x)   x
#define DB(x) //  x
#define DB2(x) //  x

using namespace asl;

#ifndef _WIN32
    #include <netinet/in.h>
    #include <unistd.h>
    #include <errno.h>
    #include <algorithm>

    #define LAST_ERROR_TYPE int

    inline
    LAST_ERROR_TYPE lastError() {
        return errno;
    }

    inline
    std::string errorDescription(LAST_ERROR_TYPE err) {
        return strerror(err);
    }
#endif

#ifdef _WIN32
    #include <process.h>
    //#define GetLastError WSAGetLastError;

    #define LAST_ERROR_TYPE DWORD

    inline
    LAST_ERROR_TYPE lastError() {
        return GetLastError();
    }

    inline
    std::string errorDescription(LAST_ERROR_TYPE err) {
        LPVOID lpMsgBuf;
        if (!FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL ))
        {
            // Handle the error.
            return std::string("unknown error code=")+asl::as_string(err);
        }
        std::string myResult = static_cast<const char*>(lpMsgBuf);
        LocalFree( lpMsgBuf );
        return myResult;
    }

#endif

using namespace std;

void
compress(const asl::ReadableBlock & inData, asl::ResizeableBlock & compressedData) {
    compressedData.resize(inData.size() * 101 / 100 + 13);
    unsigned long destLen = compressedData.size();
#if 1
    int err = compress((Bytef*)(compressedData.begin()), &destLen, (Bytef*)(inData.begin()), inData.size());
#else
    string & in_ = const_cast<string&>(inData);
    char * cp_ = &compressedData[0];
    char * ip_ = &in_[0];
    int err = compress((Bytef*)&cp_[0], &destLen, (Bytef*)&ip_[0], inData.size());
#endif
    if (err != Z_OK) {
        throw asl::Exception(string("Compress failed, error = ") + asl::as_string(err), PLUS_FILE_LINE);
    }
    compressedData.resize(destLen);
}

void
uncompress(const asl::ReadableBlock & compressedData, asl::ResizeableBlock & uncompressedData, unsigned long uncompressedSize) {
    uncompressedData.resize(uncompressedSize);
    unsigned long destLen = uncompressedData.size();
#if 1
    int err = uncompress((Bytef*)(uncompressedData.begin()), &destLen, (Bytef*)(compressedData.begin()), compressedData.size());
#else
    //int err = uncompress((Bytef*)&uncompressedData[0], &destLen, (Bytef*)&compressedData[0], compressedData.size());
    string& co_ = const_cast<string&>(compressedData);
    char* cp_ = &co_[0];
    char* up_ = &uncompressedData[0];
    int err = uncompress((Bytef*)&up_[0], &destLen, (Bytef*)&cp_[0], compressedData.size());
#endif
    if (err != Z_OK) {
        throw asl::Exception(string("Uncompress failed, error = ") + asl::as_string(err), PLUS_FILE_LINE);
    }
    if (destLen != uncompressedData.size()) {
        throw asl::Exception(string("Uncompress size differs from passed size, passed = ")
            + asl::as_string(uncompressedSize) + ", actual = " + asl::as_string(destLen),
            PLUS_FILE_LINE);
    }
}

unsigned long
Station::defaultOwnIPAddress() {
    unsigned long myAddress = 0;
    if (getenv("STATION_OWN_IP_ADDR")) {
        myAddress = asl::hostaddress(getenv("STATION_OWN_IP_ADDR"));
    } else {
        myAddress = asl::hostaddress(asl::localhostname());
    }
    return myAddress;
}

bool
Station::disableReceivingFlag() {
    return NULL != getenv("STATION_DISABLE_RECEIVING");
}


unsigned long
Station::defaultBroadcastAddress() {
    unsigned long myAddress = 0;
    if (getenv("STATION_BROADCAST_ADDR")) {
        myAddress = asl::hostaddress(getenv("STATION_BROADCAST_ADDR"));
    } else {
        myAddress = defaultOwnIPAddress() | 0xff; // default class C broadcast address
        // myAddress = defaultOwnIPAddress() | 0x1ff; // HACK: ART+COM Subnet, TODO: add proper code, see example below
    }
    return myAddress;
}

unsigned long
Station::defaultIgnoreAddress() {
    unsigned long myAddress = asl::hostaddress("127.0.0.1");
    if (getenv("STATION_IGNORE_ADDR")) {
        myAddress = asl::hostaddress(getenv("STATION_IGNORE_ADDR"));
    }
    return myAddress;
}

unsigned long
Station::defaultNetworkID() {
    unsigned long myID = 42;
    if (getenv("STATION_NETWORK_ID")) {
        myID = asl::as<unsigned long>(getenv("STATION_NETWORK_ID"));
    }
    return myID;
}

unsigned long
Station::defaultStationID() {
    unsigned long myID = getpid();
    if (getenv("STATION_ID")) {
        myID = asl::as<unsigned long>(getenv("STATION_ID"));
    }
    return myID;
}

unsigned short
Station::defaultBroadcastPort() {
    unsigned short myBroadcastPort = 9668;
    if (getenv("STATION_BROADCAST_PORT")) {
        myBroadcastPort = asl::as<unsigned short>(getenv("STATION_BROADCAST_PORT"));
    }
    return myBroadcastPort;
}

#if 0
/*
 * A little test dealie to see if we can retrieve the broadcast addr
 *
 * Works on HP_UX, freebsd, linux
 * gcc -o /tmp/get_bcast ./get_bcast.c
 *
 * Works on solaris or Unixware (SVR4) with:
 * gcc -DBSD_COMP -o /tmp/get_bcast ./get_bcast.c -lsocket -lnsl
 *
 * Doesn't seem to work at all on Digital Unix (???)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>

#include <sys/ioctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#if 0
int main (int argc, char * argv[])
{
    struct ifconf
        ifc; /* holds IOCTL return value for SIOCGIFCONF */
    int
        return_val,
        fd = -1,
        numreqs = 30,
        n;
    struct ifreq
        *ifr; /* points to one interface returned from ioctl */

    fd = socket (PF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        fprintf (stderr, "Opening socket:");
        fprintf (stderr, "got error %d (%s)\n", errno, strerror(errno));
        exit(1);
    }

    memset (&ifc, 0, sizeof(ifc));

    ifc.ifc_buf = NULL;
    ifc.ifc_len =  sizeof(struct ifreq) * numreqs;
    ifc.ifc_buf = malloc(ifc.ifc_len);

    /* This code attempts to handle an arbitrary number of interfaces,
       it keeps trying the ioctl until it comes back OK and the size
       returned is less than the size we sent it.
     */
    for (;;) {
        ifc.ifc_len = sizeof(struct ifreq) * numreqs;
        ifc.ifc_buf = realloc(ifc.ifc_buf, ifc.ifc_len);

        if ((return_val = ioctl(fd, SIOCGIFCONF, &ifc)) < 0) {
            perror("SIOCGIFCONF");
            break;
        }
        if (ifc.ifc_len == sizeof(struct ifreq) * numreqs) {
            /* assume it overflowed and try again */
            numreqs += 10;
            continue;
        }
        break;
    }

    if (return_val < 0) {
        fprintf (stderr, "ioctl:");
        fprintf (stderr, "got error %d (%s)\n", errno, strerror(errno));
        exit(1);
    }


    /* loop through interfaces returned from SIOCGIFCONF */
    ifr=ifc.ifc_req;
    for (n=0; n < ifc.ifc_len; n+=sizeof(struct ifreq)) {

        printf ("ifr_name %s\n", ifr->ifr_name);

        /* Get the flags for this interface*/
        return_val = ioctl(fd,SIOCGIFFLAGS, ifr);
        if (return_val == 0 ) {
            printf ("ifr_flags %08X\n", ifr->ifr_flags);
        } else {
            perror ("Get flags failed");
        }

        /* Get the Destination Address for this interface */
        return_val = ioctl(fd,SIOCGIFDSTADDR, ifr);
        if (return_val == 0 ) {
            if (ifr->ifr_broadaddr.sa_family == AF_INET) {
                struct sockaddr_in
                    *sin = (struct sockaddr_in *)
                    &ifr->ifr_dstaddr;

                printf ("ifr_dstaddr %s\n",
                    inet_ntoa(sin->sin_addr));

            }
            else
            {
                printf ("unsupported family for dest\n");
            }
        } else {
            perror ("Get dest failed");
        }


        /* Get the BROADCAST address */
        return_val = ioctl(fd,SIOCGIFBRDADDR, ifr);
        if (return_val == 0 ) {
            if (ifr->ifr_broadaddr.sa_family == AF_INET) {
                struct sockaddr_in
                    *sin = (struct sockaddr_in *)
                    &ifr->ifr_broadaddr;

                printf ("ifr_broadaddr %s\n",
                    inet_ntoa(sin->sin_addr));

            }
            else
            {
                printf ("unsupported family for broadcast\n");
            }

        } else {
            perror ("Get broadcast failed");
        }

        /* check the next entry returned */
        ifr++;
    }

    /* we don't need this memory any more */
    free (ifc.ifc_buf);
    close (fd);

    return 0;
}
#endif
#endif

void
Station::openStation(unsigned long theBroadcastAddress,
                     unsigned short theBroadcastPort,
                     unsigned short theReceivePort,
                     unsigned long ownIPAddress,
                     unsigned long theStationID,
                     unsigned long theNetworkID
    )
{
    _myOutgoingMessageNumber = 0;

    _myBroadcastPort = theBroadcastPort;
    _myReceivePort = theReceivePort;
    _myBroadcastAddress = theBroadcastAddress;
    _myStationID = theStationID;
    _myNetworkID = theNetworkID;
    _ownIPAddress = ownIPAddress;

    _myFileDescriptor = socket(AF_INET,SOCK_DGRAM,0);
    if (_myFileDescriptor == -1) {
        throw SocketFailed(errorDescription(lastError()),PLUS_FILE_LINE);
    }

#ifdef SO_REUSEADDR
    int myReUseAddr = -1;
    if (setsockopt(_myFileDescriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &myReUseAddr, sizeof(myReUseAddr)) < 0) {
        AC_DEBUG << "setting SO_REUSEADDR socket option";
        throw SetSockOptFailed(errorDescription(lastError()),PLUS_FILE_LINE);
    }
#endif

    int myState=-1; /*true*/
    if(setsockopt(_myFileDescriptor, SOL_SOCKET, SO_BROADCAST, (char*)&myState, sizeof(myState))!=0) {
        throw SetSockOptFailed(errorDescription(lastError()),PLUS_FILE_LINE);
    }

#ifdef _WIN32
    // set nonblocking mode in windows here
    u_long myEnableNonBlockingFlag = 1;
    int myError = ioctlsocket(_myFileDescriptor, FIONBIO, &myEnableNonBlockingFlag);
    if (myError != 0) {
        throw SetSockOptFailed(errorDescription(myError),PLUS_FILE_LINE);
    }
#endif

// Unfortunately this is not yet supported in Linux (2.4.18), may be in the future
#ifdef SO_REUSEPORT
    AC_DEBUG << "setting SO_REUSEPORT socket option";
    if(setsockopt(_myFileDescriptor, SOL_SOCKET, SO_REUSEPORT, &myState, sizeof(myState))!=0) {
        AC_DEBUG << "setting SO_REUSEPORT socket option";
        throw SetSockOptFailed(errorDescription(lastError()),PLUS_FILE_LINE);
    }
#endif

    if (!disableReceivingFlag()){
        _fromAddress.sin_family = AF_INET;
        _fromAddress.sin_addr.s_addr=htonl(INADDR_ANY);
        _fromAddress.sin_port = htons(_myReceivePort);
        AC_DEBUG << "Binding Receiver Adress " << asl::as_dotted_address(ntohl(_fromAddress.sin_addr.s_addr)) << " to port " << ntohs(_fromAddress.sin_port) << endl;

        if (bind(_myFileDescriptor,(struct sockaddr*)&_fromAddress,sizeof(_fromAddress))<0) {
            throw BindFailed(errorDescription(lastError()),PLUS_FILE_LINE);
        }
    }

    _toAddress.sin_family = AF_INET;
    _toAddress.sin_addr.s_addr=htonl(_myBroadcastAddress);
    _toAddress.sin_port = htons(_myBroadcastPort);
    AC_DEBUG << "Transmitter Adress is " << asl::as_dotted_address(ntohl(_toAddress.sin_addr.s_addr)) << " port " << ntohs(_toAddress.sin_port) << endl;

    _good = true;
}

void
Station::closeStation() {
    if (_good) {
        _good = false;
        _myOutgoingMessageNumber = 0;
#ifdef _WIN32
            closesocket(_myFileDescriptor);
#else
            ::close(_myFileDescriptor);
#endif
    }
}

void
Station::resetStatistic() {
    _myTransmitStatistic = Statistic();
    _myReceiveStatistic = Statistic();
    _myReceiveErrorStatistic = Statistic();
    _myReceiveStatisticByStation = ReceiveStatisticByStation();
}

unsigned long
Station::getLocalHostAddress() { // returns 127.0.0.1
    return 0xff000001;
}

ostream &
operator<<(ostream & os, const Station::Statistic & theStatistic) {
    os << "Messages=" << theStatistic._myMessageCount
       << " ,Packets=" << theStatistic._myPacketCount
       << " ,Bytes=" << theStatistic._myByteCount;
    return os;
};

Station::Statistic &
Station::Statistic::operator+=(const Station::Statistic & theOther) {
    _myMessageCount += theOther._myMessageCount;
    _myPacketCount += theOther._myPacketCount;
    _myByteCount += theOther._myByteCount;
    return *this;
}

Station::Statistic
operator-(const Station::Statistic & theOne, const Station::Statistic & theOther) {
    return Station::Statistic(theOne._myMessageCount - theOther._myMessageCount,
            theOne._myPacketCount - theOther._myPacketCount,
            theOne._myByteCount - theOther._myByteCount);
}

Station::Statistic
maximum(const Station::Statistic & theOne, const Station::Statistic & theOther) {
    return Station::Statistic(asl::maximum(theOne._myMessageCount, theOther._myMessageCount),
            asl::maximum(theOne._myPacketCount, theOther._myPacketCount),
            asl::maximum(theOne._myByteCount, theOther._myByteCount));
}

Station::Statistic
operator/(const Station::Statistic & theOne, double theTime) {
    return Station::Statistic(static_cast<int>(theOne._myMessageCount / theTime),
            static_cast<int>(theOne._myPacketCount / theTime),
            static_cast<int>(theOne._myByteCount / theTime) );
}



void
Station::broadcast(const asl::ReadableBlock & theData) {

    asl::ReadableBlock::size_type mySentDataBytes = 0;

    Packet myPacket;
    myPacket._myHeader._myMagicNumber = static_cast<unsigned long>(STATION_MESSAGE_MAGIC);
    myPacket._myHeader._mySrcAddress = _ownIPAddress;
    myPacket._myHeader._myStationID = _myStationID;
    myPacket._myHeader._myNetworkID = _myNetworkID;
    myPacket._myHeader._myMessageNumber = _myOutgoingMessageNumber++;

    const asl::ReadableBlock * myData = &theData;
    asl::Block compressedData;
    if (theData.size() > Packet::PAYLOAD_SIZE) {
        compress(theData,compressedData);
        myData = &compressedData;
        myPacket._myHeader._myUncompressedMessageSize = theData.size();
    } else {
        myPacket._myHeader._myUncompressedMessageSize = 0;
    }
    myPacket._myHeader._myMessageSize = myData->size();
    myPacket._myHeader._myMessagePartCount = myData->size() / Packet::PAYLOAD_SIZE + 1;
    myPacket._myHeader._myMessagePartNumber = 0;

    while (mySentDataBytes < myData->size()) {
        myPacket._myHeader._myMessagePartSize
            = std::min( static_cast<std::size_t>(myData->size() - mySentDataBytes)
                      , static_cast<std::size_t>(Packet::PAYLOAD_SIZE) );

        std::copy(myData->begin()+mySentDataBytes,
                  myData->begin()+mySentDataBytes+myPacket._myHeader._myMessagePartSize,
                  myPacket._myData);

        int myPacketSize = sizeof(Header) + myPacket._myHeader._myMessagePartSize;
        int myResult = -1;
        do {
            myResult = sendto(_myFileDescriptor, (char*)&myPacket, myPacketSize, 0, (struct sockaddr*)&_toAddress,sizeof(_toAddress));
            if (myResult < 0) {
#ifdef _WIN32
                if (lastError() != WSAEWOULDBLOCK) {
#else
                if (lastError() != EAGAIN) {
#endif
                    break;
                } else {
                    asl::msleep(1);
                }
            }
        } while (myResult < 0);
        if (myResult != myPacketSize) {
            throw SendFailed(std::string("toAddress=")+asl::as_dotted_address(ntohl(_toAddress.sin_addr.s_addr))+", port="+asl::as_string(ntohs(_toAddress.sin_port))+", error="+errorDescription(lastError()),PLUS_FILE_LINE);
        }
        mySentDataBytes += myPacket._myHeader._myMessagePartSize;

        DB(AC_TRACE << "Sent " << myResult << " bytes,"
             << " message = " <<  myPacket._myHeader._myMessageNumber
             << " ,part = " << myPacket._myHeader._myMessagePartNumber
             << " of " << myPacket._myHeader._myMessagePartCount
             << " uncompressed size " << myPacket._myHeader._myUncompressedMessageSize
             << " transmitted size " << myData->size() << endl);

        ++myPacket._myHeader._myMessagePartNumber;

        ++_myTransmitStatistic._myPacketCount;
        _myTransmitStatistic._myByteCount += myPacketSize;
    }
    ++_myTransmitStatistic._myMessageCount;
}

bool Station::receive(asl::ResizeableBlock & theData, unsigned long & theSenderAddress, unsigned long & theStationID) {

    theData.resize(0);
    for(;;) {
        asl::Ptr<Packet> myNewPacket = asl::Ptr<Packet>(new Packet);
        struct sockaddr_in mySender;
#ifdef _WIN32
        int mySenderAddrSize = sizeof(sockaddr_in);
        // for windows, non-blocking mode has been already enabled in Station::open
        int myResult = recvfrom(_myFileDescriptor, (char*)myNewPacket.get(), sizeof(Packet), 0,
                (struct sockaddr*)&mySender, &mySenderAddrSize);

        DB2(AC_TRACE << "myResult = " << myResult << ", lastError = " << lastError() << endl);
        DB(AC_TRACE << "recvfrom packet from = " << asl::as_dotted_address(ntohl(mySender.sin_addr.s_addr)) << ", port = " << mySender.sin_port << endl);
        if (myResult < 0) {
            if (lastError() == WSAEWOULDBLOCK) {
                return false;
            }
            throw RecvFailed(errorDescription(lastError()),PLUS_FILE_LINE);
        }
#else
        socklen_t mySenderAddrSize = sizeof(sockaddr_in);
        int myResult = recvfrom(_myFileDescriptor, myNewPacket.get(), sizeof(Packet), MSG_DONTWAIT,
                (struct sockaddr*)&mySender, &mySenderAddrSize);
        DB2(AC_TRACE << "myResult = " << myResult << ", lastError = " << lastError() << endl);
        if (myResult < 0) {
            if (lastError() == EAGAIN) {
                return false;
            }
            throw RecvFailed(errorDescription(lastError()),PLUS_FILE_LINE);
        }
#endif
        if (myNewPacket->_myHeader._myMagicNumber != STATION_MESSAGE_MAGIC) {
            throw InvalidPacket("Packet with bad magic number received", PLUS_FILE_LINE);
        }
        if (myNewPacket->_myHeader._myNetworkID != _myNetworkID) {
            AC_WARNING << "Packet with different Network ID (" << myNewPacket->_myHeader._myNetworkID
                 << " discarded, my ID = "<< _myNetworkID << endl;
            continue;
        }
        if (myNewPacket->_myHeader._myMessagePartSize + sizeof(Header) != static_cast<unsigned long>(myResult)) {
            throw SizeMismatch("Size in Header does not match received bytes", PLUS_FILE_LINE);
        }

        unsigned long myPacketSrcAddress = ntohl(mySender.sin_addr.s_addr);
        if ((myNewPacket->_myHeader._mySrcAddress != myPacketSrcAddress ) &&
            (myPacketSrcAddress != getLocalHostAddress()) &&
            (myPacketSrcAddress != INADDR_LOOPBACK) &&
            (myNewPacket->_myHeader._mySrcAddress !=0) )
        {
            AC_WARNING << "Packet with mismatching source address received"
                    << " IP header src = " << asl::as_dotted_address(myPacketSrcAddress)
                    << " own header src= " << asl::as_dotted_address(myNewPacket->_myHeader._mySrcAddress) << endl;

        }
        Descriptor mySenderDescriptor;
        mySenderDescriptor._separate._myIPAddress = myNewPacket->_myHeader._mySrcAddress;
        mySenderDescriptor._separate._myStationID = myNewPacket->_myHeader._myStationID;
        unsigned long long & mySenderID = mySenderDescriptor._together;

         DB(AC_TRACE << "Received " << myResult << " bytes, message=" << myNewPacket->_myHeader._myMessageNumber
             << " part = " << myNewPacket->_myHeader._myMessagePartNumber
             << " of " << myNewPacket->_myHeader._myMessagePartCount
             << " from = " << asl::as_dotted_address(mySenderID) << endl);
        if (_ownIPAddress &&
            mySenderDescriptor._separate._myIPAddress == _ownIPAddress &&
            mySenderDescriptor._separate._myStationID == _myStationID)
        {
            DB(AC_TRACE << "*** Station::receive: packet from myself ignored" << endl);
            continue;
        };
        DB2(AC_TRACE << "Packet Data = '" <<  myNewPacket->_myData << "'" << endl);

        // get a reference to our previous packet store vector
        vector<asl::Ptr<Packet> > & myStoredPackets = _myIncomingPackets[mySenderID];
        // Store our packet
        myStoredPackets.push_back(myNewPacket);

        ++_myReceiveStatistic._myPacketCount;
        _myReceiveStatistic._myByteCount += myResult;

        if (myNewPacket->_myHeader._myMessagePartNumber + 1 ==
                myNewPacket->_myHeader._myMessagePartCount )
        {
            // We received the final packet of a message, lets see if we have all parts
            //unsigned long storedPacketCount = _myIncomingPackets[mySenderID].size();
            if (myNewPacket->_myHeader._myMessagePartCount > myStoredPackets.size()) {
                // We didnt get all previous packets, lets discard them all
                DB(AC_TRACE << "Not enough packets, discarding all,  stored = " << myStoredPackets.size() << endl);
                myStoredPackets.resize(0);
                myNewPacket = asl::Ptr<Packet>();
            } else {
                DB(AC_TRACE << "Final part received, stored = " << myStoredPackets.size() << endl);
                // The number of packets is sufficient, so look if they belong to the same message
                unsigned long myMessageNumber = myNewPacket->_myHeader._myMessageNumber;
                unsigned long myPartNumber = 0;
                unsigned long myMessagePartSizeSum = 0;
                DB(AC_TRACE << "Assembling message number  " << myMessageNumber
                                << " consisting of " << myNewPacket->_myHeader._myMessagePartCount
                                << " parts, size should be = " << myNewPacket->_myHeader._myMessageSize
                                <<  endl);
                unsigned long discardedMessageNumber = 0; // just for statistic gathering
                for (vector<asl::Ptr<Packet> >::size_type i = 0; i < myStoredPackets.size(); ++i) {
                    if ((myStoredPackets[i]->_myHeader._myMessageNumber == myMessageNumber) &&
                            (myStoredPackets[i]->_myHeader._myMessagePartNumber == myPartNumber)) {
                        DB(AC_TRACE << "Found correct in-sequence packet " << i
                                << " , message=" << myStoredPackets[i]->_myHeader._myMessageNumber
                                << " part = " << myStoredPackets[i]->_myHeader._myMessagePartNumber
                                << " of " << myStoredPackets[i]->_myHeader._myMessagePartCount
                                << " , myPartNumber = " << myPartNumber
                                <<  endl);
                         ++myPartNumber; // count packet
                        myMessagePartSizeSum+= myStoredPackets[i]->_myHeader._myMessagePartSize;
                    } else {
                        // throw away out of sequence packet
                        AC_DEBUG << "Discarding out of sequence packet " << i
                                << " , message=" << myStoredPackets[i]->_myHeader._myMessageNumber
                                << " part = " << myStoredPackets[i]->_myHeader._myMessagePartNumber
                                << " of " << myStoredPackets[i]->_myHeader._myMessagePartCount
                                << " , myMessageNumber is " << myMessageNumber
                                << " , expected PartNumber is " << myPartNumber
                                <<  endl;
                       ++_myReceiveErrorStatistic._myPacketCount;
                       _myReceiveErrorStatistic._myByteCount += myStoredPackets[i]->_myHeader._myMessagePartSize;
                       if (myStoredPackets[i]->_myHeader._myMessagePartNumber != discardedMessageNumber) {
                            ++_myReceiveErrorStatistic._myMessageCount;
                            discardedMessageNumber = myStoredPackets[i]->_myHeader._myMessagePartNumber;
                       }
                       myStoredPackets.erase(myStoredPackets.begin()+i);
                       --i;
                    }
                }
                // If we still have the right amount of packets, they are in order and
                // complete; lets assemble them now
                if (myStoredPackets.size() == myNewPacket->_myHeader._myMessagePartCount) {
                    // ..but check the sizes for validity before we allocate anything
                    if (myMessagePartSizeSum != myNewPacket->_myHeader._myMessageSize) {
                        throw SizeMismatch(
                                "Sum of part sizes did not match transmitted header value",
                                PLUS_FILE_LINE);
                    }
                    DB(AC_TRACE << "Sizes correct, assembled message size will be " << myMessagePartSizeSum << endl);
                    // we should be safe now to allocate the result string
                    theData.resize(myMessagePartSizeSum);
                    // ..and assemble it finally
                    //string::iterator myResult = theData.begin();
                    unsigned char * myResult = &theData[0];
                    for (vector<asl::Ptr<Packet> >::size_type i = 0; i < myStoredPackets.size(); ++i) {
                        memcpy(myResult,&myStoredPackets[i]->_myData,myStoredPackets[i]->_myHeader._myMessagePartSize);
                        myResult+=myStoredPackets[i]->_myHeader._myMessagePartSize;
                        //myResult = copy(&myStoredPackets[i]->_myData,
                        //                &myStoredPackets[i]->_myData + myStoredPackets[i]->_myHeader._myMessagePartSize,
                          //              myResult);
                    }
                    // throw away our stored packets
                    myStoredPackets.resize(0);
                    // ... and set other return values before returning
                    theSenderAddress =mySenderDescriptor._separate._myIPAddress;
                    theStationID = mySenderDescriptor._separate._myStationID;

                    if (myNewPacket->_myHeader._myUncompressedMessageSize) {
                        asl::Block compressedData = theData;
                        uncompress(compressedData,theData, myNewPacket->_myHeader._myUncompressedMessageSize);
                        DB(AC_TRACE << "Uncompressed Message size " <<compressedData.size() <<"->"<< theData .size()<< endl);
                    }
                    DB2(AC_TRACE << "Received '" << theData << "'" << endl);
                    DB( if (theData.size()> 20000) { AC_TRACE << "Received large Message:'" << theData << "'" << endl;} );
                    ++_myReceiveStatistic._myMessageCount;
                    return true;
                } else {
                    DB(AC_TRACE << "Could not assemble packet because some parts have been missing:"
                            << ", stored packets = " << myStoredPackets.size()
                            << ", required = " << myNewPacket->_myHeader._myMessagePartCount
                            << endl);
                }
            }
        }
    }
}


