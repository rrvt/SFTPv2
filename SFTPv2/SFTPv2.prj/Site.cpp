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


//volatile UnitDsc* Site::lastDsc;

static UINT updateThrd(   void* param);             // Thread for processing an update

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

  switch (dsc->unitOp) {

    case NilOp      : break;
    case PutOp      : put(*dsc);      break;
    case GetOp      : get(*dsc);      break;
    case DelOp      : del(*dsc);      break;
    case DelDirOp   : delDir(*dsc);   break;
    default         : break;
    }
  }


LRESULT Site::finUpdate(WPARAM wparam, LPARAM lParam) {
String path;

  mainFrm()->closePrgBar();

  display(updateList);   doc()->saveBaseLine();

  sendDisplayMsg();   Sleep(1);   sendWdwScroll(false);   return 0;
  }


void Site::get(UnitDsc& ud) {
UnitDsc* dsc;

  if (!LocalSite::createDir(ud.key.path)) return;

  if (!RemoteSite::loadTransport(ud.key.path)) return;

  if (!LocalSite::storTransport(ud.key.path)) return;

  getAttr(ud.key.path, ud.size, ud.date);   ud.unitOp = GetDone;

  dsc = baseLineList.add(ud);   dsc->unitOp = NilOp;
  dsc = localDirList.add(ud);   dsc->unitOp = NilOp;

  sendStepPrgBar();
  }



void Site::put(UnitDsc& ud) {
UnitDsc* dsc;

  if (!RemoteSite::createDir(ud.key.path)) return;

  if (!LocalSite::loadTransport(ud.key.path)) return;

  if (!RemoteSite::storTransport(ud.key.path)) return;

  ud.unitOp = PutDone;

  if (!webDirList.isEmpty()) {dsc = webDirList.add(ud);  dsc->unitOp = NilOp;}
  dsc = baseLineList.add(ud);   dsc->unitOp = NilOp;

  sendStepPrgBar();
  }


// Del file in remote host

void Site::del(UnitDsc& ud) {
  if (!RemoteSite::del(ud.key.path)) return;

  ud.unitOp = DelDone;

  if (!webDirList.isEmpty()) {webDirList.delRcd(ud);}
  baseLineList.delRcd(ud);

  sendStepPrgBar();
  }


// Del dir in remote host

void Site::delDir(UnitDsc& ud) {
  ud.unitOp = DelDirDone;

  if (!RemoteSite::delDir(ud.key.path)) return;
  baseLineList.delRcd(ud);

  sendStepPrgBar();
  }


void Site::display(UnitList& ul) {
UnitListIter iter(ul);
UnitDsc*     dsc;
int          n = 0;
int          m = 0;
String       op;

  notePad << nClrTabs << nSetTab(35) << nSetTab(47);

  for (dsc = iter(); dsc; dsc = iter++) {

    switch (dsc->unitOp) {

      case NilOp        : op.clear();                  m++;   break;
      case PutOp        : op = _T("Put");              n++;   break;
      case GetOp        : op = _T("Get");              n++;   break;
      case DelOp        : op = _T("Del");              n++;   break;
      case DelDirOp     : op = _T("Del Dir");          n++;   break;

      case PutDone      : op = _T("Put");              n++;   break;
      case GetDone      : op = _T("Get");              n++;   break;
      case DelDone      : op = _T("Delete Rmt File");  n++;   break;
      case DelDirDone   : op = _T("Delete Rmt Dir");   n++;   break;

      default           : continue;
      }

    notePad << dsc->key.path;
    if (n) {
      notePad << nTab << op;
      }

    if (m) {
      String t = dsc->key.dir ? _T("d") : _T("f");
      notePad << nTab << t;
      }
    notePad << nCrlf;
    }

  notePad << nCrlf << nTab;

  if (n > 0) {
    notePad << n << _T(" file");   if (n > 1) notePad << _T("s");
    notePad << _T(" updated");
    }

  else if (m > 0) {
    notePad << m << _T(" entr");
    if (m == 1) notePad << _T("y");   else notePad << _T("ies");
    }

  else notePad << _T("No Entries");

  notePad << nCrlf;

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

  if (dsc && dsc->status != NilSts && !dsc->createWebDir(noFiles)) return;

  lclPath = siteID.localRoot  + key.path;

  if (!sftpSSL.getLocalFile(lclPath)) return;

  webPath = siteID.remoteRoot + key.path;

  if (!sftpSSL.stor(toRemotePath(webPath))) return;

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
#if 1
#else
String localPath;
  localPath = LocalSite::getPath(ud.key.path);
  if (!doc()->storeXfrBuffer(localPath)) return false;
#endif
#if 1
#else
String localPath;
  localPath = LocalSite::getPath(ud.key.path);

  if (!doc()->loadXfrBuffer(localPath)) return;
#endif

