// Transport Data


#include "pch.h"
#include "SftpTransport.h"
#include "FileIO.h"
#include "NotePad.h"
#include "SftpDataIter.h"
#include "SftpErr.h"
#include "SftpSSLi.h"
#include "SftpSSLv2.h"


SftpTransport sftpTransport;


typedef struct linger  Linger;


bool SftpTransport::open(TransportMode md, TCchar* arg) {
int     pos;
String  s;
TCchar* cp;
int     v[6];
Sin     sin;

  mode = md;

  if (!sftpSSLi.sendCmd(_T("TYPE"), mode.typeStg(), 200)) return false;

  if (!sftpSSLi.sendCmd(_T("PASV"), 227)) return false;

  pos = sftpSSLi.lastResp.find('(');    if (pos < 0) return false;

  s = sftpSSLi.lastResp.substr(pos+1);  cp = s.str();

  _stscanf_s(cp, _T("%u,%u,%u,%u,%u,%u"), &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);

  ZeroMemory(&sin, sizeof(sin) );
  sin.in.sin_family = AF_INET;

  sin.sa.sa_data[2] = char(v[2]);
  sin.sa.sa_data[3] = char(v[3]);
  sin.sa.sa_data[4] = char(v[4]);
  sin.sa.sa_data[5] = char(v[5]);
  sin.sa.sa_data[0] = char(v[0]);
  sin.sa.sa_data[1] = char(v[1]);

  if (!sftpSSLi.sendCmd(mode.command(), arg))
                            {sftpErr.put(_T("Passive Transport Command Failure"));  return false;}

  if (!open(sin)) {sftpErr.put(_T("Passive Transport Connection Failed"));   return false;}

  return sftpSSLi.readRsp(150);           // 150 - File status okay; about to open data connection
  }


bool SftpTransport::open(Sin& sin) {
Linger lng = {0, 1000};
int    i;
int    rslt;

  if (!SftpSocket::create()) return false;

  if (setsockopt(skt, SOL_SOCKET, SO_LINGER, (Cchar*) &lng, sizeof(lng)) == -1)
              {sftpErr.wsa(_T("Transport Set Linger"));       SftpSocket::close();   return false;}

  for (i = 0; i < 5; i++) {
    rslt = ::connect(skt, &sin.sa, sizeof(sin.sa));   if (!rslt) return true;
    Sleep(10);
    }

  sftpErr.wsa(_T("Transport Connect Failed.."));   return false;
  }


bool SftpTransport::read() {            // SSLFileType flTyp    fileType = flTyp;

  if (!isOpen()) return false;

  return SftpSocket::read();
  }


// Compute the number of bytes in the store buffer

int  SftpTransport::nBytes() {
SftpStrIter iter(*this);
SftpBlock*  blk;
int         n;

  for (n = 0, blk = iter(); blk; blk = iter++) n += blk->n;

  return n;
  }


bool SftpTransport::write() {
SftpStrIter iter(*this);
SftpBlock*  blk;

  for (blk = iter(); blk; blk = iter++)
                          if (blk->n > 0 && SftpSocket::write(blk->bfr, blk->n) <= 0) return false;
  return true;
  }



// load buffer from a local file represented by ar

void SftpTransport::load(Archive& ar) {
SftpBlock* blk;

  SftpStore::clear();

  for (blk = data.allocate(); blk; blk = data.allocate()) {
    blk->n = sizeof(FtpBfr);

    if (!ar.readBlk(blk->bfr, blk->n)) {data.deallocate(blk); break;}

    data += blk;
    }
  }



// Store buffer content to a local file represented by archive

void SftpTransport::store(Archive& ar) {

  switch (mode.isAscii()) {
    case true : storeAscii(ar); break;
    case false: storeImage(ar); break;
    }
  }


void SftpTransport::storeAscii(Archive& ar) {
SftpDataIter ix(*this);
String*      s;
String       crlf = _T("\r\n");

  for (s = ix(); s; s = ix++) {ar.write(*s);   ar.write(crlf);}
  }


void SftpTransport::storeImage(Archive& ar) {
SftpStrIter iter(*this);
SftpBlock*  blk;

  for (blk = iter(); blk; blk = iter++) if (!ar.write(blk->bfr, blk->n)) break;
  }


// close Transport

void SftpTransport::close() {
TransportMode md = mode();    if (!md) return;

  switch (md) {
    case ListMd :
    case GetAMd :
    case GetIMd : break;
    case PutAMd :
    case PutIMd : shutDown();
    default     : break;
    }

  SftpSocket::close();

  switch (md) {
    case ListMd :
    case GetAMd : break;
    case PutAMd :
    case PutIMd :
    case GetIMd : sftpSSLi.readRsp(226); break;
    default     : break;
    }

  mode.clear();
  }



