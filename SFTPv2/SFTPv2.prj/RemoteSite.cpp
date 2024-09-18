// Remote Site details


#include "pch.h"
#include "RemoteSite.h"
#include "AppUtilities.h"
#include "Date.h"
#include "FileName.h"
#include "IniFile.h"
#include "NotePad.h"
#include "SftpSSLv2.h"
#include "SFTPv2.h"
#include "SFTPv2Doc.h"
#include "Site.h"
#include "Utilities.h"


static TCchar* URLNameKey    = _T("URLName");
static TCchar* RemoteRootKey = _T("RemoteRoot");


void RemoteSite::closeTransport() {sftpSSL.closeTransport();}


bool RemoteSite::load(TCchar* sect) {
  if   (!iniFile.read(sect, URLNameKey,    url,  _T(""))) return false;
  return iniFile.read(sect, RemoteRootKey, root, _T(""));
  }


bool RemoteSite::save(TCchar* sect) {
  if   (!iniFile.write(sect, URLNameKey,    url)) return false;
  return iniFile.write(sect, RemoteRootKey, root);
  }




bool RemoteSite::login() {
String   name;
String   pswd;
int      i;
bool     opened;

  if (loggedIn) return true;

  if (!isValid()) return false;

  if (!getNmPswd(name, pswd)) return false;

  for (i = 0, opened = sftpSSL.open(url); i < 2 && !opened; i++, opened = sftpSSL.open(url))
                                                                                          continue;
  if (!opened) return false;

  loggedIn = sftpSSL.login(name, pswd);   expunge(name);   expunge(pswd);

  if (!loggedIn) {sftpSSL.noop();   return false;}

  notePad << _T("Web Site: ") << site.name << _T(" is Logged In") << nCrlf;

  return true;
  }


bool RemoteSite::getNmPswd(String& name, String& pswd) {
CNGblock blk;
CNG      cng;
TCchar*  tc;
String   np;
int      pos;

  doc()->loadCNG(blk);    tc = cng(blk, MyPassword);   if (!tc) return false;

  np = tc;   pos = np.find(_T(';'));   if (pos <= 0) return false;

  name = np.substr(0, pos);   pswd = np.substr(pos+1);

  blk.expunge();   return true;
  }


void RemoteSite::logout() {if (loggedIn) {sftpSSL.close();  loggedIn = false;}}


// All fields not empty

bool RemoteSite::isValid() {

  if (site.name.isEmpty())       return false;
  if (url.isEmpty())        return false;
  if (root.isEmpty()) return false;

  return true;
  }




String RemoteSite::ensureSite(TCchar* name) {
String s = name;   s.lowerCase();
String t = name;
int    pos;

  pos = s.find(_T("site"));   if (pos >= 0) t = t.substr(0, pos);
  pos = s.find(_T("web"));    if (pos >= 0) t = t.substr(0, pos);

  t.trim();   t += _T(" Web Site");   t.trim();   return t;
  }


bool RemoteSite::createDir(TCchar* relPath) {
String fullPath = getPath(relPath);             fullPath = root + fullPath;
String path     = toRemote(fullPath);
bool   rslt;
String stk[16];
int    stkX;
String right;

  if (sftpSSL.cwd(path)) return true;

  for (rslt = rmvLastDir(path, right, _T('/')), stkX = 0; rslt;
                                                        rslt = rmvLastDir(path, right, _T('/'))) {
    stk[stkX++] = right;   if (sftpSSL.cwd(path)) break;
    }

  for (stkX--; stkX >= 0; stkX--)
    {path += stk[stkX];   if (!sftpSSL.mkd(path)) return false;}

  return true;
  }


void RemoteSite::getRmtAttr(TCchar* relPath, int& size, Date& date) {
String path = root + relPath;   path = toRemote(path);

  sftpSSL.size(path, size);   sftpSSL.date(path, date);
  }


bool RemoteSite::loadTransport(TCchar* relPath)
    {String path = root + relPath;   path = toRemote(path);   return sftpSSL.retr(toRemote(path));}


bool RemoteSite::storTransport(TCchar* relPath)
                             {String path = root + relPath;   return sftpSSL.stor(toRemote(path));}


bool RemoteSite::delDir(TCchar* relPath)
               {String path = root + relPath;   path = toRemote(path);   return sftpSSL.rmd(path);}


bool RemoteSite::del(TCchar* relPath)
              {String path = root + relPath;   path = toRemote(path);    return sftpSSL.del(path);}



///------------

#if 0
bool SendEMail::login(CNGblock& blk, TCchar* key) {
CNG      cng;
TCchar*  tc = cng(blk, key);   if (!tc) return false;
String   np = tc;
int      pos;
String   name;
String   pswd;
bool     rslt;

  pos = np.find(_T(';'));   if (pos <= 0) return false;

  name = np.substr(0, pos);   pswd = np.substr(pos+1);

  blk.expunge();

  rslt = eMail.login(_T("gmail.com"), _T("587"), name, pswd);

  expunge(name);   expunge(pswd);   return rslt;
  }
#endif
//  if (userID.isEmpty())     return false;
//  if (password.isEmpty())   return false;

