// Transport Data


#include "pch.h"
#include "SftpTransport.h"
#include "FileIO.h"
#include "NotePad.h"
#include "SftpDataIter.h"
#include "SftpErr.h"
#include "SftpSSLi.h"


SftpTransport sftpTransport;


typedef struct linger  Linger;


bool SftpTransport::initPassiveMode(TCchar* cmd, TCchar* arg) {
int     pos;
String  s;
TCchar* cp;
int     v[6];
Sin     sin;

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

  if (!sftpSSLi.sendCmd(cmd, arg))
                            {sftpErr.put(_T("Passive Transport Command Failure"));  return false;}

  if (!open(sin)) {sftpErr.put(_T("Passive Transport Connection Failed"));   return false;}

  return sftpSSLi.readRsp(150);           // 150 - File status okay; about to open data connection
  }


bool SftpTransport::open(Sin& sin) {
Linger lng = {0, 1000};

  if (!SftpSocket::create()) return false;

  if (setsockopt(skt, SOL_SOCKET, SO_LINGER, (Cchar*) &lng, sizeof(lng)) == -1)
              {sftpErr.wsa(_T("Transport Set Linger"));       SftpSocket::close();   return false;}

  if (::connect(skt, &sin.sa, sizeof(sin.sa)))
              {sftpErr.wsa(_T("Transport Connect Failed"));   SftpSocket::close();   return false;}
  return true;
  }


bool SftpTransport::read(SSLFileType flTyp) {

  if (!isOpen()) return false;

  fileType = flTyp;   return SftpSocket::read();
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

#if 0
// load store buffer from local file

bool SftpTransport::load(TCchar* path) {
FileIO   fio;
SftpBlock* blk;

  clear();

  if (!fio.open(path, FileIO::Read)) return false;

  for (blk = data.allocate(); blk; blk = data.allocate()) {
    blk->n = sizeof(FtpBfr);

    if (!fio.read(blk->bfr, blk->n)) {data.deallocate(blk); break;}

    data += blk;
    }

  fio.close();   return data.end() > 0;
  }


// stote store buffer in a local file

bool SftpTransport::store(TCchar* path) {
FileIO      fio;
SftpStrIter iter(*this);
SftpBlock*  blk;
int         mode = FileIO::Write | FileIO::Create;
bool        rslt = true;

  if (!fio.open(path, (FileIO::OpenParms) mode)) return false;

  for (blk = iter(); blk; blk = iter++) {
    if (!fio.write(blk->bfr, blk->n)) {rslt = false; break;}
    }

  fio.close();   return rslt;
  }
#endif


// load buffer from a local file represented by ar

void SftpTransport::load(Archive& ar) {
SftpBlock* blk;

  clear();

  for (blk = data.allocate(); blk; blk = data.allocate()) {
    blk->n = sizeof(FtpBfr);

    if (!ar.readBlk(blk->bfr, blk->n)) {data.deallocate(blk); break;}

    data += blk;
    }
  }



// Store buffer content to a local file represented by archive

void SftpTransport::store(Archive& ar) {

  switch (fileType) {
    case AsciiFlTyp : storeAscii(ar); break;
    case ImageFlTyp : storeImage(ar); break;
    default         : String s = ar.getFilePath(); break;
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
SftpBlock*    blk;

  for (blk = iter(); blk; blk = iter++)
    if (!ar.write(blk->bfr, blk->n)) break;
  }



///------------------------

#if 0


bool SftpTransport::open(Sin& sin) {
Linger lng = {0, 1000};

  if (!sftpOps.openSkt(_T("Transport"))) return false;

  if (setsockopt(skt, SOL_SOCKET, SO_LINGER, (Cchar*) &lng, sizeof(lng)) == -1)
                                  {err.wsa(_T("Transport Set Linger"));       closeSkt();   return false;}

  if (::connect(skt, &sin.sa, sizeof(sin.sa)))
                                  {err.wsa(_T("Transport Connect Failed"));   closeSkt();   return false;}
  return true;
  }
#endif

