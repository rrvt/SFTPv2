// SSH FTP Interface


#include "pch.h"
#include "SftpSSLv2.h"
#include "Date.h"
#include "NotePad.h"
#include "SftpErr.h"
#include "SftpSocket.h"
#include "SftpSSLi.h"
#include "SftpTransport.h"


SftpSSL sftpSSL;


static const int Retries = 3;


SftpSSL::SftpSSL() {
int wsaErr = WSAStartup(MAKEWORD(2, 2), &wsa);              // Required in Windows

  if (wsaErr) {sftpErr.skt(_T("WSAStartup() failed"), wsaErr);}
  }


// Opens Socket to site

bool SftpSSL::open(TCchar* host) {return sftpSSLi.open(host);}


void SftpSSL::close() {sftpSSLi.close();   sftpTransport.close();}


bool SftpSSL::login(TCchar* userId, TCchar* password) {

  if (!sftpSSLi.sendCmd(_T("USER"), userId, 331)) return false;
                                                            // 331 - User name okay, need password
  return sftpSSLi.sendCmd(_T("PASS"), password, 230);       // 230 - User logged in, proceed.
  }


bool SftpSSL::avbl(String& avail) {
int i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("AVBL"), 0, avail); i++) Sleep(10);

  return i < Retries;
  }


bool SftpSSL::noop() {
int i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("NOOP"), 0, 200); i++) Sleep(10);

  return i < Retries;
  }


bool SftpSSL::stat(String& rslt) {
int i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("STAT"), 0, rslt); i++) Sleep(10);

  return i < Retries;
  }


// return true and the current remote directory or false if it fails

bool SftpSSL::pwd(String& rslt) {
int    i;
String s;
int    pos;
int    pos2;

  for (i = 0;
      i < Retries && sftpSSLi.sendCmd(_T("PWD"), 0, s) && s.find(_T("257")) != 0; i++) Sleep(10);

  if (i >= Retries) return false;

  pos = s.find(_T('"')) + 1;    pos2 = s.find(_T('"'), pos);

  rslt = s.substr(pos, pos2 - pos);   return true;
  }


// Change Working Directory

bool SftpSSL::cwd(TCchar* dir) {
int    i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("CWD"), dir, 250); i++) Sleep(10);

  return i < Retries;
  }


// Make Directory

bool SftpSSL::mkd(TCchar* dir) {
int    i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("MKD"), dir, 257); i++) Sleep(10);

  return i < Retries;
  }


// Remove Directory

bool SftpSSL::rmd( TCchar* dir) {
int i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("RMD"), dir, 250); i++) Sleep(10);

  return i < Retries;
  }


// Remove Directory Tree

bool SftpSSL::rmda(TCchar* dir) {
int    i;

  for (i = 0; i < Retries && !sftpSSLi.sendCmd(_T("RMDA"), dir, 250); i++) Sleep(10);

  return i < Retries;
  }


// Return true when file is deleted, else return false

bool SftpSSL::del(TCchar* webPath) {return sftpSSLi.sendCmd(_T("DELE"), webPath, 250);}


// Returns true when the Unix like directory list (each line contains a line for each entity in the
// current directory.  The data is stored in the data store which can be read later.

bool SftpSSL::list(TCchar* path, TCchar* args, SftpStore& store) {
String cmd = _T("LIST ");   cmd += args;
bool   rslt;

  sftpTransport.setType(AsciiFlTyp);

  if (!sftpTransport.initPassiveMode(cmd, path)) return false;

  rslt = sftpTransport.read(AsciiFlTyp);

  store -= sftpTransport;

  sftpTransport.close();   return rslt;
  }


// Return true when the size of the file is found, else return false

bool SftpSSL::size(TCchar* path, int& size) {
int  code;
uint pos;

  if (!sftpSSLi.sendCmd(_T("SIZE"), path, 213)) return false;         // 213 - File status

  code = sftpSSLi.lastResp.stoi(pos);

  if (code != 213 || sftpSSLi.lastResp[pos] != _T(' ')) return false;

  size = sftpSSLi.lastResp.substr(pos+1).stoi(pos);   return true;
  }


// return true when the date of the file is found, else return false

bool SftpSSL::date(TCchar* path, Date& val) {
int  code;
uint pos;

  if (!sftpSSLi.sendCmd(_T("MDTM"), path, 213)) return false;         // 213 - File status.

  code = sftpSSLi.lastResp.stoi(pos);

  if (code != 213 || sftpSSLi.lastResp[pos] != _T(' ')) return false;

  try {ToDate to(sftpSSLi.lastResp.substr(pos+1));    val = to();}
    catch (...) {notePad << _T("Failed Date Interpretation: ") << sftpSSLi.lastResp << nCrlf;
    }

  val.toLocalTime(val);   return true;
  }


// Sending a file to the web and Retrieving a file from the web is performed in three steps:
//   *  Loading a local file into a transport buffer or
//   *  Retrieving a remote file into a transport buffer
//
//   *  Storing the content of the buffer in the remote host or
//   *  Storing the content of the buffer in the local PC
//
//   *  Closing the Transport

// load transport buffer from a local

void SftpSSL::load(Archive& ar) {return sftpTransport.load(ar);}


// Copy sftpTransport store to web host file at webPath

bool SftpSSL::stor(TCchar* webPath) {
SSLFileType flTyp = sftpFileType(webPath);
bool        rslt;

  sftpSSLi.setType(flTyp);

  if (!sftpTransport.initPassiveMode(_T("STOR"), webPath)) return false;

  rslt = sftpTransport.write();

  return rslt & sftpSSLi.readRsp(226);            // 226 - Closing data connection. Requested file
  }                                               // action successful


// copy transport buffer to unique file name in current direcotry

bool SftpSSL::stou(TCchar* webPath, String& fileName) {
SSLFileType flTyp = sftpFileType(webPath);
int         pos;
bool        rslt;

  sftpSSLi.setType(flTyp);

  if (!sftpTransport.initPassiveMode(_T("STOU"), 0)) return false;

  pos = sftpSSLi.firstResp.find(_T("FILE: "));

  if (pos >= 0) fileName = sftpSSLi.firstResp.substr(pos + 6);

  rslt = sftpTransport.write();   sftpTransport.close();   sftpTransport.clear();

  return rslt & sftpSSLi.readRsp(226);            // 226 - Closing data connection. Requested file
  }                                               // action successful


// Copy sftpTransport store to web host file at webPath

bool SftpSSL::append(TCchar* webPath) {
SSLFileType flTyp = sftpFileType(webPath);
bool        rslt;

  sftpSSLi.setType(flTyp);

  if (!sftpTransport.initPassiveMode(_T("APPE"), webPath)) return false;

  rslt = sftpTransport.write();

  return rslt & sftpSSLi.readRsp(_T('2'));
  }


// copy file from web host to sftpTransport buffer

bool SftpSSL::retr(TCchar* webPath) {
SSLFileType flTyp = sftpFileType(webPath);
bool        rslt;

  sftpTransport.clear();

  sftpSSLi.setType(flTyp);

  if (!sftpTransport.initPassiveMode(_T("RETR"), webPath)) return false;

  rslt = sftpTransport.read(flTyp);   return rslt;
  }


void SftpSSL::store(Archive& ar) {sftpTransport.store(ar);}     // Store file in the sftpTransport


void SftpSSL::closeTransport() {sftpTransport.close();}         // Close Transport transaction



void SftpSSL::openSSLThreadStop() {OPENSSL_thread_stop();}




//-----------------------

//SftpStore& SftpSSL::fileData() {return sftpTransport.sftpOps;}
//#include "SftpOps.h"

