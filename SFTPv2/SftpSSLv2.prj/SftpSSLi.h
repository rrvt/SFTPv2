// Secure Socket Layer Interface


#pragma once
#include "SftpSocket.h"
#include "SftpStore.h"
#include "SftpUtilities.h"
#include <openssl/ssl.h>

class SftpBlock;


class SftpSSLi : public SftpSocket {
SSL*       ssl;                             // The OpenSSL library objects needed for
SSL_CTX*   ctx;                             // encrypted communication
BIO*       sbio;

public:

          SftpSSLi() : ssl(0), ctx(0), sbio(0) { }
         ~SftpSSLi() {close();}

  void    clrLast() {}

  bool    open(TCchar* host);
  void    close();

  SSL*    getNewSSL();
  SSLRslt connect();

  bool    sendCmd(TCchar* cmd, int code);
  bool    sendCmd(TCchar* cmd, TCchar* args, int code);
  bool    sendCmd(TCchar* cmd, TCchar* args, String& response);
  bool    sendCmd(TCchar* cmd, TCchar* arg);
  bool    sendCmd(TCchar* cmd);
  int     write(Cchar* buf, int noBytes);

  bool    readRsp(int code);
  bool    readRsp(Tchar ch);
  bool    readRsp(String& rsp);
  bool    read();

private:

  SSLrslt readEncrpt(SftpBlock& ftpBlk);
  SSLrslt readSSL(   SftpBlock& blk);

  void    closeSSL();
  };


extern SftpSSLi sftpSSLi;



///--------------------

//String   firstResp;                       // First Response String
//String   lastResp;                        // Last Response String
  //Tchar    lastCh;
//String   lastCmd;                         // Last Command Sent

//int        cnt;

