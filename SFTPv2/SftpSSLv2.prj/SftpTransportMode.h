// SftpTransport Mode -- A combination of Transport command and file type (ascii or image)
// This little class determines:
//   * ascii or image transfer type
//   * a necessary operation at close for some of the operations (read image and all put
//     operations)
//   * the ftp command that is to be executed in the transport module

#pragma once
#include "SftpSSLv2.h"

// The order is important in the TransportMode enum.  Everything to the left (i.e. less than) PutImd
// are ascii transfers, to the right and including PutImd are image transfers.

enum TransportMode {NilMd = 0, ListMd, PutAMd, GetAMd, PutIMd, GetIMd};


class SftpTransportMode {

TransportMode mode;

public:

                     SftpTransportMode() : mode(NilMd) { }
                     SftpTransportMode(SftpTransportMode& md) {mode = md.mode;}
                    ~SftpTransportMode() { }

  void               clear()   {mode = NilMd;}
  bool               isEmpty() {return mode == NilMd;}

  SftpTransportMode& operator= (SftpTransportMode& md) {mode = md.mode; return *this;}
  SftpTransportMode& operator= (TransportMode    md) {mode = md;      return *this;}

  TransportMode      operator() () {return mode;}

  void               set(SftpIO io, TCchar* path = 0);
  TransportMode      get(SftpIO io, TCchar* path = 0);

  TCchar*            typeStg();
  bool               isAscii();
  TCchar*            command();
  };

