// Transport Data


#pragma once
#include "SftpSocket.h"
#include "SftpSSLv2.h"
#include "SftpStore.h"
#include "SftpUtilities.h"
#include "SftpTransportMode.h"

class SftpErr;
class Archive;


typedef union {
struct sockaddr    sa;
struct sockaddr_in in;
} Sin;


class SftpTransport : public SftpSocket {

SftpTransportMode mode;

public:

  SftpTransport() { }
 ~SftpTransport() {close();}

  bool open(TransportMode md, TCchar* arg);             // Open Transport for moving data to/from web
                                                  // from/to  store
  bool read();                                    // Read data from web host into store
  bool write();                                   // Write data in store to web host

  void close();                                   // close Transport

  void load( Archive& ar);                        // load/store buffer from/to a local file
  void store(Archive& ar);                        // represented by ar

  int  nBytes();                                  // Compute the number of bytes in the store buffer
                                                  // archive
private:

  bool open(Sin& sin);

  void storeAscii(Archive& ar);
  void storeImage(Archive& ar);
  };


extern SftpTransport sftpTransport;




