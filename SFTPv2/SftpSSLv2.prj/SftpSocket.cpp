


#include "pch.h"
#include "SftpSocket.h"
#include "SftpErr.h"
#include "SftpUtilities.h"
#include <ws2tcpip.h>


typedef struct servent      ServerEnt;
typedef struct in_addr      InAddr;
typedef struct addrinfo     AddrInfo;
typedef short               AddrFamily;
typedef struct sockaddr_in6 SockAddrIn6;


bool SftpSocket::create() {
int on = 1;

  if (skt) close();

  skt = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (skt == INVALID_SOCKET)
                        {sftpErr.wsa(eMsg(_T("Create"), _T("Socket")));   skt = 0;   return false;}

  if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (Cchar*) &on, sizeof(on)) == -1)
                                   {sftpErr.wsa(_T("Set Sock Option"));   close();   return false;}
  return true;
  }


bool SftpSocket::open(TCchar* host) {
//int         on = 1;
String      web;
SockAddrIn4 sin;
char        lhost[1024];
char*       pos;

#if 1
  if (!create()) return false;
#else
  if (skt) close();

  skt = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (skt == INVALID_SOCKET) {sftpErr.wsa(eMsg(host, _T("Socket")));   skt = 0;   return false;}

  if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (Cchar*) &on, sizeof(on)) == -1)
                                   {sftpErr.wsa(_T("Set Sock Option"));   close();   return false;}
#endif
  memset(&sin, 0, sizeof(sin));   sin.sin_family = AF_INET;

  web = _T("ftp."); web += host;   ToAnsi h(web);

  strcpy_s(lhost, sizeof(lhost), h());    pos = strchr(lhost, ':');

  if (!getServerByName("ftp", sin)) return false;

  if (!inetAddr(lhost, sin.sin_addr.s_addr) &&
                                         !findHostIPaddr(lhost, sin.sin_addr.s_addr)) return false;

  if (::connect(skt, (SockAddr*) &sin, sizeof(sin)))
                                           {sftpErr.wsa(_T("connect"));   close();   return false;}
  return true;
  }


bool SftpSocket::getServerByName(Cchar* service, SockAddrIn4& sin) {
ServerEnt* pse = getservbyname(service, "tcp");

  if (!pse) {sftpErr.wsa(_T("getservbyname"));  return false;}

  sin.sin_port = pse->s_port;

  int x = pse->s_port;

  return true;
  }


bool SftpSocket::inetAddr(Cchar* p, u_long& addr) {
InAddr inAddr;

  if (inet_pton(AF_INET, p, &inAddr) <= 0)  return false;

  addr = inAddr.S_un.S_addr;    return true;
  }


bool SftpSocket::findHostIPaddr(Cchar* url, u_long& addr) {
AddrInfo     hints;
AddrInfo*    info;
char         stringBuf[46];
AddrFamily   family;
SockAddrIn4* sockaddr_ipv4;
SockAddrIn6* sockaddr_ipv6;
int          rslt;

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  rslt = GetAddrInfoA(url, "http", &hints, &info);

  if (rslt) {
    ToUniCode uni(url);
    String    s = _T("findHostIPaddr: ");   s += uni();

    sftpErr.skt(s, rslt);  return false;
    }

  stringBuf[0] = 0;  family = info->ai_family;

  switch (family) {
    case AF_UNSPEC: sftpErr.put(_T("Family Unspecified"));   return false;

    case AF_INET  : sockaddr_ipv4 = (SockAddrIn4*) info->ai_addr;

                    addr = sockaddr_ipv4->sin_addr.s_addr; break;

    case AF_INET6 : sockaddr_ipv6 = (SockAddrIn6*) info->ai_addr;

                    sockaddr_ipv6->sin6_addr;

                    sftpErr.put(_T("Family: IP6 address"));  return false;

                    break;

    default       : sftpErr.put(_T("Gsmily: Other format")); return false;
    }

  return true;
  }


void SftpSocket::close() {

  if (!skt) return;

  if (lastOp == WriteOp) {if (shutdown(skt, SD_SEND)) sftpErr.wsa(_T("Shutdown"));   read();}

  closesocket(skt);   skt = 0;   clear();
  }


bool SftpSocket::sendCmd(TCchar* cmd, int code) {
  if (!sendCmd(cmd)) return false;

  return readRsp(code);
  }


bool SftpSocket::sendCmd(TCchar* cmd, TCchar* args, int code) {
  if (!sendCmd(cmd, args)) return false;

  return readRsp(code);
  }


bool SftpSocket::sendCmd(TCchar* cmd, TCchar* args, String& response) {

  if (!sendCmd(cmd, args)) return false;

  return readRsp(response);
  }


// The command and arguments are combined (provided the arg pointer is not zero) and the command
// is sent.  Clears the response buffer and returns result of send the command

bool SftpSocket::sendCmd(TCchar* cmd, TCchar* arg) {

  if (!arg) return sendCmd(cmd);

  String s = cmd;   s += _T(' ');   s += arg;   return sendCmd(s);
  }


bool SftpSocket::sendCmd(TCchar* cmd) {
String s = cmd;  s += _T("\r\n");

  ToAnsi buf(s);   return write(buf(), buf.length()) == buf.length();
  }


// Write the data in the buffer to the web host

int SftpSocket::write(Cchar* buf, int noBytes) {
int n;

  lastOp = WriteOp;   n = send(buf, noBytes);   return n >= 0 ? n : 0;
  }


bool SftpSocket::readRsp(int code) {

  clrLast();   if (!read()) return false;

  return find(code);
  }


bool SftpSocket::readRsp(String& rsp) {

  if (!read()) return false;

  getResp(rsp);   return true;
  }


bool SftpSocket::read() {
int        i;
SftpBlock* blk;

  data.clear();   blkd(true);   lastOp = ReadOp;

  for (i = 0, blk = 0; ; i++) {
    blk = data.allocate();

    switch (readClr(*blk)) {
      case SSLtrue    : data[i].p = blk; continue;
      case SSLtimeout :
      case SSLfalse   : if (blk->n) data[i].p = blk;
                        else        data.deallocate(blk);
                        return nData() > 0;
      }
    }
  }


SSLrslt SftpSocket::readClr(SftpBlock& ftpBlk) {
SSLrslt r;                                                      // && SSL_has_pending(ssl)

  ftpBlk.n = ftpBlk.curX = 0;

  for (r = readSkt(ftpBlk); r == SSLtrue && ftpBlk.n < sizeof(FtpBfr); r = readSkt(ftpBlk))
                                                                       {blkd(false);   Sleep(100);}
  return ftpBlk.n == sizeof(FtpBfr) ? SSLtrue : r;
  }


SSLrslt SftpSocket::readSkt(SftpBlock& blk) {
char* bfr  = blk.bfr;
int   size = sizeof(FtpBfr);
int   n;

  setTimeout(5);

  n = recv(skt, bfr + blk.n, size - blk.n, 0);

  if (!n)       return SSLfalse;

  if (n < 0)    return SSLtimeout;

  blk.n += n;   return SSLtrue;
  }


// enables or disables the blocking mode for the
// socket based on the numerical value of iMode.
// If iMode = 0, blocking is enabled;
// If iMode != 0, non-blocking mode is enabled.
/*
  iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
  if (iResult != NO_ERROR) printf("ioctlsocket failed with error: %ld\n", iResult);
*/

bool SftpSocket::blkd(bool blocking) {
ulong arg = blocking ? 0 : 1;

  if (!skt) return false;

  if (!ioctlsocket(skt, FIONBIO, &arg)) return true;

  sftpErr.wsa(_T("ioctlsocket failed with error")); return false;
  }


bool SftpSocket::setTimeout(int noSecs) {
struct timeval tv   = {noSecs * 1000, 0};

  if (setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (Cchar*) &tv, sizeof(tv)) == -1)
                               {sftpErr.wsa(_T("Set Sock Timeout"));   close();   return false;}
  return true;
  }




///---------------

//if (!skt.open(_T("Command"))) return false;
//#include "SftpBlock.h"
//#include "SftpDataIter.h"

