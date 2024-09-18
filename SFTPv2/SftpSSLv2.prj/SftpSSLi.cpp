// Secure Socket Layer Interface


#include "pch.h"
#include "SftpSSLi.h"
#include "IsSSLErr.h"


SftpSSLi sftpSSLi;


bool SftpSSLi::open(TCchar* host) {

  close();

  if (!SftpSocket::open(host)) return false;

  ssl = getNewSSL();  if (!ssl) return false;

  if (!SftpSocket::readRsp(220)) {close(); return false;}             // 220 - Service ready for new user.

  if (!SftpSocket::sendCmd(_T("AUTH TLS"), 234)) {close(); return false;}
                                                          // 234 - Server accepts the security
                                                          // mechanism specified by the client;
  switch (sftpSSLi.connect()) {                           // no security data needs to be
    case SSLFail    : return false;                       // exchanged.  After connect all commands
    case SSLWantRead: readRsp(_T('2'));                   // need to go to SSL
    }

  return true;
  }


#if 0


void SftpOps::closeSSL(int& cnt) {
int rslt;

  if (ssl) {

    rslt = SSL_shutdown(ssl);   cnt++;

    if (rslt == 0 && cnt < 2) {closeSSL(cnt);  return;}

    SSL_free(ssl);
    }

  if (ctx) SSL_CTX_free(ctx);

  ssl = 0;   ctx = 0;   sbio = 0;   clear();   lastOp = NilOp;
  }


void SftpOps::closeSkt() {

  if (!skt) return;

  if (lastOp == WriteOp) {if (shutdown(skt, SD_SEND)) err.wsa(_T("Shutdown"));   read();}

  closesocket(skt);   skt = 0;
  }
#endif


void SftpSSLi::close() {closeSSL();   SftpSocket::close();}


void SftpSSLi::closeSSL() {
int i;

  if (!SftpSocket::isOpen()) return;

  blkd(true);

  if (ssl) {
    for (i= 0; i < 3 && !SSL_shutdown(ssl); i++) continue;
    SSL_free(ssl);   ssl = 0;
    }

  if (ctx) {SSL_CTX_free(ctx);   ctx = 0;}

  sbio = 0;
  }



SSL* SftpSSLi::getNewSSL() {

  if (ssl)                   return ssl;
  if (!SftpSocket::isOpen()) return 0;

  ctx = SSL_CTX_new(TLS_client_method());   SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

  SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

  return SSL_new(ctx);
  }



SSLRslt SftpSSLi::connect() {
int rslt;

  sbio = BIO_new_socket(skt, BIO_NOCLOSE);

  SSL_set_bio(ssl, sbio, sbio);

  blkd(true);   rslt = SSL_connect(ssl);     if (rslt != 1) return sslRslt(ssl, rslt);

  return SSLSuccess;
  }


bool SftpSSLi::sendCmd(TCchar* cmd, int code) {

  if (!sendCmd(cmd)) return false;

  return readRsp(code);
  }


bool SftpSSLi::sendCmd(TCchar* cmd, TCchar* args, int code) {

  if (!sendCmd(cmd, args)) return false;

  return readRsp(code);
  }


bool SftpSSLi::sendCmd(TCchar* cmd, TCchar* args, String& response) {

  if (!sendCmd(cmd, args)) return false;

  return readRsp(response);
  }


// The command and arguments are combined (provided the arg pointer is not zero) and the command
// is sent.  Clears the response buffer and returns result of send the command


bool SftpSSLi::sendCmd(TCchar* cmd, TCchar* arg) {

  if (!arg) return sendCmd(cmd);

  String s = cmd;   s += _T(' ');   s += arg;   return sendCmd(s);
  }


bool SftpSSLi::sendCmd(TCchar* cmd) {
String s = cmd;   s += _T("\r\n");

ToAnsi buf(s);   return write(buf(), buf.length()) == buf.length();}


// Write the data in the buffer to the web host using the correct method

int SftpSSLi::write(Cchar* buf, int noBytes) {
int n;

  n = SSL_write(ssl,  buf, noBytes);

  return sslRslt(ssl, n) ? n : 0;
  }



bool SftpSSLi::readRsp(int code) {

  clrLast();   if (!read()) return false;

  return find(code);
  }


bool SftpSSLi::readRsp(Tchar ch) {

  clrLast();   if (!read()) return false;

  return find(ch);
  }


bool SftpSSLi::readRsp(String& rsp) {

  if (!read()) return false;

  getResp(rsp);  return true;
  }


bool SftpSSLi::read() {
int      i;
SftpBlock* blk;

  data.clear();   blkd(true);

  for (i = 0, blk = 0; ; i++) {
    blk = data.allocate();

    switch (readEncrpt(*blk)) {
      case SSLtrue    : data[i].p = blk; continue;
      case SSLtimeout :
      case SSLfalse   : if (blk->n) data[i].p = blk;
                        else        data.deallocate(blk);
                        return nData() > 0;
      }
    }
  }


SSLrslt SftpSSLi::readEncrpt(SftpBlock& ftpBlk) {
SSLrslt r;                                                      // && SSL_has_pending(ssl)

  for (r = readSSL(ftpBlk); r == SSLtrue && ftpBlk.n < sizeof(FtpBfr);
                                                    r = readSSL(ftpBlk)) {blkd(false); Sleep(10);}
  return ftpBlk.n == sizeof(FtpBfr) ? SSLtrue : r;
  }


SSLrslt SftpSSLi::readSSL(SftpBlock& blk) {
char*  bfr  = blk.bfr;
int    size = sizeof(FtpBfr);
size_t n;
int    rslt;

  setTimeout(5);

  rslt = SSL_read_ex(ssl, bfr + blk.n, size - blk.n, &n);

  if (rslt) {blk.n += n;  return SSLtrue;}

  if (SSL_get_error(ssl, rslt) == SSL_ERROR_SYSCALL && WSAGetLastError() == WSAETIMEDOUT)
                                                                                 return SSLtimeout;
  return SSLfalse;
  }




////////-------------------

#if 0
SftpDataIter ix(*this);
String*      s;
ulong        v;
uint         nxt;
bool         rslt;
  for (rslt = false, s = ix(); s; s = ix++) {
    String t = *s;

    v = t.stoul(nxt);    if (nxt > 4) continue;

    if (t[0] == ch) {

      if (!rslt) {rslt = true;   firstResp = t;   lastCh = t[0];}

      lastResp = t;
      }
    }
#endif
//String s = cmd;  if (args) {s += _T(' ');   s += args;}
//#include "SftpDataIter.h"

