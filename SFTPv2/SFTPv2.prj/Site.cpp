// Site Details


#include "pch.h"
#include "Site.h"
#include "AppUtilities.h"
#include "CNG.h"
#include "IniFile.h"
#include "MessageBox.h"
#include "NotePad.h"
#include "Resource.h"
#include "SFTPv2.h"
#include "SFTPv2Doc.h"
#include "SiteLoginDlg.h"
#include "UnitList.h"
#include "Utilities.h"
#include "WorkerThrd.h"


volatile UnitDsc* Site::lastDsc;

static UINT updateThrd(void* param);            // Thread for processing an update

TCchar* GlobalSect  = _T("Global");
TCchar* LastSiteKey = _T("LastSite");
TCchar* MyPassword  = _T("Secure File Transfer Protocol v2");

static TCchar* SiteNameKey = _T("SiteName");                    // IniFile key words


Site site;


bool Site::setLastSite() {

  if (name.isEmpty()) return false;

  iniFile.write(GlobalSect, LastSiteKey,   name);   return true;
  }


bool Site::loadData(String& sect) {
String user;
String pwd;

  if (sect.isEmpty()) return false;

  if (!iniFile.read(sect, SiteNameKey, name, _T(""))) return false;
  if (!LocalSite::load(sect)) return false;
  if (!RemoteSite::load(sect)) return false;

  return true;
  }


bool Site::saveData() {
String user;
String pwd;

  if (!setLastSite()) return false;

  if (!iniFile.write(name, SiteNameKey,   name)) return false;
  if (!LocalSite::save(name)) return false;
  if (!RemoteSite::save(name)) return false;
  return true;
  }


bool Site::edit() {
String       userID;
String       pswd;
SiteLoginDlg dlg;
String       np;
String       path;
CNG          cng;
CNGblock*    cngBlk;
int          pos;

  dlg.name = name;   dlg.rootTxt = site.lclRoot();   dlg.urlName = url;
  dlg.remoteRoot = site.rmtRoot();

  if (getNmPswd(userID, pswd)) {dlg.userID = userID;   dlg.password = pswd;}

  if (dlg.DoModal() == IDOK) {

    if (dlg.userID.find(_T(';')) >= 0)
                                     {messageBox(_T("Name may not include a ';'"));  return false;}
    name = dlg.name;   site.lclRoot() = dlg.rootTxt;   url = dlg.urlName;   url.trim();

    pos = url.find(_T("ftp."));   if (pos >= 0) url = url.substr(pos+4);

    site.setRmtRoot(dlg.remoteRoot);   name = ensureSite(name);

    np = dlg.userID + _T(";") + dlg.password;

    cngBlk = cng(np, MyPassword);

    expunge(np);
    expunge(dlg.userID);
    expunge(dlg.password);

    doc()->saveCNG(cngBlk);    cngBlk->expunge();   return site.saveData();
    }

  return false;
  }


void Site::update() {
int noFiles;

  noFiles = 0;

  if (!login()) return;

  notePad << _T("Update Files") << nCrlf << nCrlf;

//  display();   return;

  mainFrm()->startPrgBar(updateList.nData());

  workerThrd.start(updateThrd, (void*) &updateList, ID_UpdateMsg);
  }


UINT updateThrd(void* param) {                      // Start Thread to do the work

  sendWdwScroll();

  if (updateList.isEmpty()) return false;

  return site.doXfer(*(UnitList*) param) ? 0 : 1;
  }


bool Site::doXfer(UnitList& ul) {
UnitListIter iter(ul);
UnitDsc*     dsc;
String       lclPath;
String       webPath;
String       rslt;

  for (dsc = iter(); dsc; dsc = iter++) if (dsc->unitOp <  DelOp)    performOp(dsc);
  for (dsc = iter(); dsc; dsc = iter++) if (dsc->unitOp <  DelDirOp) performOp(dsc);
  for (dsc = iter(); dsc; dsc = iter++) if (dsc->unitOp <= DelDirOp) performOp(dsc);
  return true;
  }


// Perform operation

void Site::performOp(UnitDsc* dsc) {

  lastDsc = dsc;

  switch (dsc->unitOp) {

    case NilOp      : break;
    case PutOp      : put(*dsc);      lastDsc->unitOp = PutDone;      break;
    case GetOp      : get(*dsc);      lastDsc->unitOp = GetDone;      break;
    case DelOp      : del(*dsc);      lastDsc->unitOp = DelDone;      break;
    case DelDirOp   : delDir(*dsc);   lastDsc->unitOp = DelDirDone;   break;
    case MkLclDirOp : break;
    case MkRmtDirOp : break;
    default         : break;
    }
  }


LRESULT Site::finUpdate(WPARAM wparam, LPARAM lParam) {
String path;

  mainFrm()->closePrgBar();

  display();

 // if (noFiles) doc()->saveData(, siteID.dbPath(path));

  sendDisplayMsg();   Sleep(1);   sendWdwScroll(false);   return 0;
  }


bool Site::get(UnitDsc& ud) {
String localPath;

  if (!LocalSite::createDir(ud.key.path)) return false;

  if (!RemoteSite::loadTransport(ud.key.path)) return false;

  localPath = LocalSite::getPath(ud.key.path);
  if (!doc()->storeXfrBuffer(localPath)) return false;

  closeTransport();

  getAttr(ud.key.path, ud.size, ud.date);   ud.unitOp = GetDone;

  UnitDsc* dsc = baseLineList.add(ud);   dsc->unitOp = NilOp;

  //sendStepPrgBar();

  return true;
  }



bool Site::put(UnitDsc& ud) {
String localPath;

  if (!RemoteSite::createDir(ud.key.path)) return false;

  localPath = LocalSite::getPath(ud.key.path);

  if (!doc()->loadXfrBuffer(localPath)) return false;

  if (!RemoteSite::storTransport(ud.key.path)) return false;

  ud.unitOp = PutDone;

  closeTransport();   //sendStepPrgBar();

  ud.unitOp = PutDone;

  return true;
  }


// Del file in remote host

bool Site::del(UnitDsc& ud) {return RemoteSite::del(ud.key.path);}


// Del dir in remote host

bool Site::delDir(UnitDsc& ud) {return RemoteSite::delDir(ud.key.path);}


void Site::display() {
UnitListIter iter(updateList);
UnitDsc*     dsc;
int          n;

  notePad << nClrTabs << nSetTab(15);

  for (dsc = iter(), n = 0; dsc; dsc = iter++) {

    switch (dsc->unitOp) {

      case NilOp      : continue;
      case PutOp      : notePad << _T("Put");            n++;   break;
      case GetOp      : notePad << _T("Get");            n++;   break;
      case DelOp      : notePad << _T("Del");            n++;   break;
      case DelDirOp   : notePad << _T("Del Dir");        n++;   break;
      case MkLclDirOp : notePad << _T("Make Lcl Dir");   n++;   break;
      case MkRmtDirOp : notePad << _T("Make Rmt Dir");   n++;   break;
      default         : continue;
      }

    notePad << nTab << dsc->key.path<< nCrlf;
    }

  if      (n <= 0) notePad << _T("No Files");
  else if (n == 1) notePad << n << _T(" file");
  else                   notePad << n << _T(" files");

  notePad << _T(" updated") << nCrlf;

  sendDisplayMsg();
  }


String Site::dataFileName() {
String s = name;
String t;
int    pos;

  for (pos = s.find(' '); pos >= 0; pos = s.find(_T(' '))) {
    t += s.substr(0, pos);   s = s.substr(pos+1);
    }
  return t + s;
  }


// Returns a relative local address

String  Site::toRelative(TCchar* path) {
String s     = path;
bool   isWeb = s.find(_T('/')) >= 0;
String root  = isWeb ? RemoteSite::root : LocalSite::root;

  if (s.find(root) == 0) s = s.substr(root.length());

  return toLocal(s);
  }


// Returns a full path from a local address, either a web address or local

String  Site::toFull(TCchar* relPath, bool isWeb) {
  }



////--------------

#if 0
NamePswdDlg dlg;
String      np;
String      path;
CNG         cng;

  while (dlg.DoModal() == IDOK) {
    notePad << _T("Name: ") << dlg.name << _T(", Password: ") << dlg.pswd << nCrlf;

    if (dlg.pswd.find(_T(';')) >= 0 || dlg.name.find(_T(';') >= 0) >= 0) continue;

    np = dlg.name + _T(";") + dlg.pswd;

    cngBlk = cng(np, MyPassword);

    expunge(np);
    expunge(dlg.name);
    expunge(dlg.pswd);

    path = theApp.roamingPath() + _T("NamePassword.cng");

    dataSource = NamePswdSrc;   pathDlgDsc(_T("Save Password"), path, _T("cng"), _T("*.cng"));

    if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);

    cngBlk->expunge();   break;
    }

  display();
#endif
#if 1
#else
    path = theApp.roamingPath() + _T("NamePassword.cng");

    dataSource = NamePswdSrc;   pathDlgDsc(_T("Save Password"), path, _T("cng"), _T("*.cng"));

    if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);

    cngBlk->expunge();   break;
#endif
#if 1
#else
String lclPath;
String webPath;

  dsc = curFileDscs.findDir(key.path);

  if (dsc && dsc->status != NilSts && !dsc->createWebDir(noFiles)) return false;

  lclPath = siteID.localRoot  + key.path;

  if (!sftpSSL.getLocalFile(lclPath)) return false;

  webPath = siteID.remoteRoot + key.path;

  if (!sftpSSL.stor(toRemotePath(webPath))) return false;

     noFiles++;   updated = true;   return true;        //webFiles.modified();
#endif
#if 0
      case WebPutSts:
      case DifPutSts: if (!dsc->put(noFiles)) continue;

                      webFiles.modified();    break;

      case GetSts   : if (!dsc->get(noFiles)) continue;   break;

      case DelSts   : if (!dsc->del(noFiles)) continue;

                      webFiles.modified();    break;

      default       : continue;
#endif

