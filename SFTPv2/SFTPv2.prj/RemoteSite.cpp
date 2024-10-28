// Remote Site details


#include "pch.h"
#include "RemoteSite.h"
#include "AppUtilities.h"
#include "Date.h"
#include "IniFile.h"
#include "LexT.h"
#include "NotePad.h"
#include "SFTPv2.h"
#include "SFTPv2Doc.h"
#include "Site.h"
#include "StringInput.h"
#include "UnitList.h"
#include "Utilities.h"
#include "WorkerThrd.h"

//#include "MessageBox.h"           // Debugging


static UINT getWebDirThrd(void* param);

typedef LexT<StringInput, true, false, false> Lex;


static TCchar* URLNameKey    = _T("URLName");
static TCchar* RemoteRootKey = _T("RemoteRoot");


bool RemoteSite::load(TCchar* sect) {
String s;
  if (!iniFile.read(sect, URLNameKey,    url,  _T(""))) return false;
  if (!iniFile.read(sect, RemoteRootKey,   s,  _T(""))) return false;
  rmtXform.set(s);   return true;
  }


bool RemoteSite::save(TCchar* sect) {
  if   (!iniFile.write(sect, URLNameKey,    url)) return false;
  return iniFile.write(sect, RemoteRootKey, rmtXform.get());
  }


bool RemoteSite::login() {
String name;
String pswd;

  if (loggedIn) return true;

  if (!isValid()) return false;

  if (!doc()->loadNamePassword(site.name, name, pswd)) return false;

  loggedIn = sftpSSL.login(url, name, pswd);   expunge(name);   expunge(pswd);

  if (!loggedIn) {sftpSSL.noop();   return false;}

  notePad << nSetTab(40);

  notePad << _T("Web Site: ") << site.name << _T(" is Logged In") << nCrlf;

  return true;
  }


void RemoteSite::logout() {if (loggedIn) {sftpSSL.close();  loggedIn = false;}}


void RemoteSite::compSites() {
int noFiles;

  noFiles = 0;

  if (!login()) return;

  mainFrm()->startPrgBar(baseLineList.nData());

  workerThrd.start(getWebDirThrd, (void*) &webDirList, ID_CompSitesMsg);
  }


LRESULT RemoteSite::onCompSites(WPARAM wparam, LPARAM lParam) {
String path;

  mainFrm()->closePrgBar();

  notePad.clear();

  doc()->comparePresences();       notePad << nCrlf;

  notePad << _T("Update List") << nCrlf << nCrlf;
  site.display(updateList);    notePad << nCrlf;

  sendDisplayMsg();   Sleep(1);   sendWdwScroll(false);   return 0;
  }


void RemoteSite::getRmtSite() {
int noFiles;

  noFiles = 0;

  if (!login()) return;

  mainFrm()->startPrgBar(baseLineList.nData());

  workerThrd.start(getWebDirThrd, (void*) &webDirList, ID_DspRmtSiteMsg);
  }



LRESULT RemoteSite::onDspRmtSite(WPARAM wparam, LPARAM lParam) {
String path;

  mainFrm()->closePrgBar();

  doc()->dspRmtSite();

  sendDisplayMsg();   Sleep(1);   sendWdwScroll(false);   return 0;
  }


UINT getWebDirThrd( void* param) {

  sendWdwScroll();

  return site.doRmtDir(site.rmtRoot(), *(UnitList*) param) ? 0 : 1;
  }


bool RemoteSite::doRmtDir(TCchar* fullPath, UnitList& ul) {
String       curPath = fullPath;
SftpStore    store;
SftpDataIter iter(store);
String*      s;

  sftpSSL.list(curPath, store);

  for (s = iter(); s; s = iter++) {
    UnitDsc dsc;

    if (parse(*s, curPath, dsc)) {

      ul.add(dsc);   sendStepPrgBar();

      if (dsc.key.dir) {String path = fullDirPath(dsc.key.path);   doRmtDir(path, ul);}
      }
    }

  return true;
  }


bool RemoteSite::parse(String& line, TCchar* path, UnitDsc& item) {
Lex    lex;
Token* tok;
Token* tok1;
int    i;
int    noWhite;
bool   collect;
String attr;
String name;
String s;

  lex.initialize(); lex.input.set(line);

  for (i = 0, noWhite = 0, collect = false; lex.get_token() != EOFToken; i++) {
    tok  = lex.token;
    tok1 = lex.token1;

    if (!i) attr = tok->name;

    if (collect)         {name += tok->name;   lex.accept_token();   continue;}

    if (tok->code == WhiteToken) {noWhite++;   lex.accept_token();   continue;}

    if (noWhite < 8)                          {lex.accept_token();   continue;}

    if (tok->code == PeriodToken || tok->name[0] == _T('_')) break;

    name = tok->name;    collect = true;       lex.accept_token();
    }

  if (!collect) return false;

  item.key.dir  = attr[0] == _T('d');
  s             = path + name;   if (item.key.dir) s = rmtXform.trmntDir(s);
  item.key.path = rmtXform.normalize(s);
  item.name     = name;
  item.relPath  = ::getPath(item.key.path);
  if (!item.key.dir) getRmtAttr(item.key.path, item.size, item.date);

  return true;
  }


// All fields not empty

bool RemoteSite::isValid() {

  if (site.name.isEmpty())  return false;
  if (url.isEmpty())        return false;
  if (rmtXform.isEmpty())   return false;

  return true;
  }


bool RemoteSite::createDir(TCchar* relPath) {
String path = fullDirPath(relPath);
String stk[16];
int    stkX = 0;
String right;

  while (!path.isEmpty()) {
    if (sftpSSL.cwd(path) || !rmvLastDir(path, right, _T('/'))) break;

    stk[stkX++] = right;
    }

  for (stkX--; stkX >= 0; stkX--) {path += stk[stkX];   if (!sftpSSL.mkd(path)) return false;}

  return true;
  }


void RemoteSite::getRmtAttr(TCchar* relPath, int& size, Date& date) {
String path = fullFilePath(relPath);

  sftpSSL.size(path, size);   sftpSSL.date(path, date);
  }


bool RemoteSite::get(TCchar* relPath) {
String pth = relPath;

  if (!openTransport(GetSftpIO, fullFilePath(pth))) {closeTransport();   return false;}

    if (!sftpSSL.readTransport())                   {closeTransport();   return false;}

    if (!site.storTransport(pth))                   {closeTransport();   return false;}

  closeTransport();    return true;
  }


bool RemoteSite::put(TCchar* relPath) {

  if (!createDir(relPath)) return false;

  if (!openTransport(PutSftpIO, fullFilePath(relPath))) {closeTransport();   return false;}

    if (!site.loadTransport(relPath))                   {closeTransport();   return false;}

    if (!sftpSSL.writeTransport())                      {closeTransport();   return false;}

  closeTransport();   return true;
  }


bool RemoteSite::delDir(TCchar* relPath) {return sftpSSL.rmd(fullFilePath(relPath));}


bool RemoteSite::del(TCchar* relPath)    {return sftpSSL.del(fullFilePath(relPath));}



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
#if 1
#else
bool   rslt;

  if (sftpSSL.cwd(path)) return true;

  for (rslt = rmvLastDir(path, right, _T('/')), stkX = 0; rslt;
                                                        rslt = rmvLastDir(path, right, _T('/'))) {
    stk[stkX++] = right;   if (sftpSSL.cwd(path)) break;
    }
#endif
#if 0


bool WebFiles::loadOne(TCchar* path, WebNode& webNode) {
String       pth = path;
SftpDataIter iter(sftpSSL.fileData());
String*      line;

  sftpSSL.list(path);    webNode.path = fixRemotePath(pth);

  indent += 3;   notePad << nClrTabs << nSetTab(indent) << nSetTab(indent + 30);

  for (line = iter(); line; line = iter++) {
    WebItem item;   if (parse(*line, webNode.path, item)) webNode.data = item;

    if (item.typ == WebFileType) sendStepPrgBar();
    }

  WebNodeIter iterWN(webNode);
  WebItem*    item;

  for (item = iterWN(); item; item = iterWN++) {
    if (item->typ == WebDirType)
         {NewAlloc(WebNode);   item->node = AllocNode;   loadOne(webNode.path + item->name, *item->node);}
    }

  indent -= 3;   return webNode.nData() > 0;
  }



bool WebFiles::parse(String& line, TCchar* path, WebItem& item) {
Lex    lex;
Token* t;
int    i;
int    noWhite;
bool   collect;
String attr;
String name;

  lex.initialize(); lex.input.set(line);

  for (i = 0, noWhite = 0, collect = false; lex.get_token() != EOFToken; i++) {
    t = lex.token;

    if (!i) attr = t->name;

    if (collect)         {name += t->name;  lex.accept_token();   continue;}

    if (t->code == WhiteToken) {noWhite++;   lex.accept_token();   continue;}

    if (noWhite < 8)                        {lex.accept_token();   continue;}

    if (t->code == PeriodToken || t->name[0] == _T('_')) break;

    name = t->name;    collect = true;   lex.accept_token();
    }

  if (!collect) return false;

  item.typ  = attr[0] == _T('d') ? WebDirType : WebFileType;
  item.name = name;
  if (item.typ == WebFileType) sftpSSL.size(path + item.name, item.size);

  return true;
  }
#endif

#if 0
//int      i;
//bool     opened;
  for (i = 0, opened = sftpSSL.open(url); i < 2 && !opened; i++, opened = sftpSSL.open(url))
                                                                                          continue;
  if (!opened) return false;
#endif


//bool RemoteSite::loadTransport(TCchar* relPath) {return sftpSSL.retr(fullFilePath(relPath));}
#if 0
// Returns a relative local address

String& RemoteSite::toRelative(TCchar* fullPath) {

  path = site.normalizePath(fullPath);

  if (path.find(root) == 0) path = path.substr(root.length());

  return path;
  }
#endif
#if 1

//void    RemoteSite::setRoot(TCchar* path) {rmtXform.set(rmtXform.toLocal(path));}
//String& RemoteSite::getRoot()      {return rmtXform.toRemote(rmtXform.get());}

#else
void    RemoteSite::setRoot(TCchar* path) {root = site.LocalSite::toLocal(path);}
String& RemoteSite::getRoot() {return fixSeparators(root);}
#endif


// Read from web host into store

//bool RemoteSite::readTransport() {return sftpSSL.readTransport();}


// Write from store into web host

//bool RemoteSite::writeTransport(TCchar* relPath) {return sftpSSL.writeTransport();}


//bool RemoteSite::storTransport(TCchar* relPath) {return sftpSSL.stor(fullFilePath(relPath));}
#if 0
TCchar ch = item.key.path[0];
if (ch < _T(' ') || _T('~') < ch) {
String t;  t.format(_T("count = %i"), count);
  messageBox(t);
  }
#endif
//String& RemoteSite::fullDirPath(TCchar* relPath) {return fullFilePath(::getPath(relPath));}


//String&  RemoteSite::fullFilePath(TCchar* relPath)
//                                               {path = root + relPath;   return fixSeparators();}

#if 0
String& RemoteSite::fixSeparators() {
int    n = path.length();
int    i;

  for (i = 0; i < n; i++) if (path[i] == _T('\\')) path[i] = _T('/');

  return path;
  }
#endif


