// Transport Data


#pragma once
#include "SftpSocket.h"
#include "SftpStore.h"
#include "SftpUtilities.h"

class SftpErr;
class Archive;


typedef union {
struct sockaddr    sa;
struct sockaddr_in in;
} Sin;


class SftpTransport : public SftpSocket {         //  : public SftpStore
public:

  SftpTransport() { }
 ~SftpTransport() {close();}

  void clear() {SftpSocket::clear();}

  bool initPassiveMode(TCchar* cmd, TCchar* arg);

  bool open(Sin& sin);
  void close() {SftpSocket::close();}

  bool read(SSLFileType flTyp);             // Read everything from web host into store

  int  nBytes();                            // Compute the number of bytes in the store buffer

  bool write();                             // Write everything in store to web host

//  bool load(TCchar* path);                // load store buffer from local file
//  bool store(TCchar* path);               // store store buffer in a local file
  void load(Archive& ar);                   // load buffer from a local file represented by ar
  void store(Archive& ar);                  // Store buffer content to a local file represented by
                                            // archive
private:

  void storeAscii(Archive& ar);
  void storeImage(Archive& ar);
  };


extern SftpTransport sftpTransport;




//----------------------

//  SftpTransport() : sftpOps(*(SftpOps*)0), err(*(SftpErr*)0), skt(*(Socket*)0) { }

//SftpOps& sftpOps;

//Socket&  skt;                             // Windows Socket

