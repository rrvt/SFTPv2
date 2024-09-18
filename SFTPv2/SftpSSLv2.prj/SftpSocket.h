// Socket used in sftpSSL



#pragma once
#include "SftpStore.h"
#include "SftpUtilities.h"


class SftpBlock;


enum LastOp {NilOp, ReadOp, WriteOp};

typedef struct sockaddr_in SockAddrIn4;


class SftpSocket : public SftpStore {
protected:

Socket  skt;                              // Windows Socket

public:

LastOp  lastOp;                           // Last Operation

  SftpSocket() : skt(0), lastOp(NilOp) { }

  bool    create();
  bool    open(TCchar* host);
  void    close();

  void    clear() {SftpStore::clear();  lastOp = NilOp;}
  void    clrLast() {lastOp = NilOp; clearLast();}

  bool    isOpen() {return skt != 0;}
//  Socket  operator() () {return skt;}

  bool    sendCmd(TCchar* cmd, int code);
  bool    sendCmd(TCchar* cmd, TCchar* args, int code);
  bool    sendCmd(TCchar* cmd, TCchar* args, String& response);
  bool    sendCmd(TCchar* cmd, TCchar* args);
  bool    sendCmd(TCchar* cmd);
  int     write(Cchar* buf, int noBytes);
  int     send(Cchar* buf, int len) {return ::send(skt, buf, len, 0);}

  bool    readRsp(int code);
  bool    readRsp(String& rsp);
  bool    read();

  bool    blkd(bool blocking);
  bool    setTimeout(int noSecs);

//  void    setLastOp(LastOp lOp = NilOp) {lastOp = lOp;}

private:

  SSLrslt readClr(SftpBlock& ftpBlk);
  SSLrslt readSkt(SftpBlock& blk);
  bool    getServerByName(Cchar* service, SockAddrIn4& sin);
  bool    inetAddr(Cchar* p, u_long& addr);
  bool    findHostIPaddr(Cchar* url, u_long& addr);
  };






///----------------------

//String  firstResp;                        // First Response String
//String  lastResp;                         // Last Response String
//Tchar   lastCh;                           // Last initial char of Response String
  //SSLrslt lastReadRslt;                     // Last read() result

