

#include "pch.h"
#include "UnitList.h"
#include "AppUtilities.h"
#include "Archive.h"
#include "CSVLex.h"
#include "CSVOut.h"
#include "DirList.h"
#include "FileName.h"
#include "FileSrch.h"
#include "Resource.h"
#include "Site.h"


static TCchar* Version = _T("1");

UnitList baseLineList;              // Representation of WebDirList after an update
UnitList localDirList;              // List of files and directories on local PC
UnitList webDirList;                // List of files and directories on the remote host
UnitList updateList;                // List of files and directories to update next


void UnitList::copy(UnitList& sf) {
UnitListIter iter(sf);
UnitDsc* dsc;

  clear();   loaded = sf.loaded;

  for (dsc = iter(); dsc; dsc = iter++) {UnitDsc* p = add(*dsc);   p->clrOp();}
  }




bool UnitList::loadFromPC() {
String& path = site.lclRoot();

  if (path.isEmpty()) return false;

  clear();   loadOneDir(path);   return !isEmpty();
  }


void UnitList::loadOneDir(TCchar* path) {
Expandable<String, 2> data;
FileSrch              srch;
String                sub;
String                file;
int                   i;
int                   n;

  addDir(path);

  if (srch.findAllSubDirs(path)) while (srch.getName(sub)) data = sub;

  if (srch.findAllFiles(path)) while (srch.getName(file)) addFile(file);

  for (i = 0, n = data.end(); i < n; i++) loadOneDir(data[i]);

  data.clear();
  }



UnitDsc* UnitList::add(UnitKey& key)
                         {String path = key.path;   return key.dir ? addDir(path) : addFile(path);}


UnitDsc* UnitList::addFile(TCchar* path) {
String   pth   = path;
bool     isWeb = pth.find(_T('/')) >= 0;
UnitDsc* ud;

  pth = site.toRelative(path);    if (pth.isEmpty() || filterFile(pth)) return 0;

  ud = find(pth);   if (ud) return ud;

  ud  = data.allocate();

  if (isWeb) site.getRmtAttr(path, ud->size, ud->date);
  else       site.getAttr(   pth,  ud->size, ud->date);

  ud->key.path = pth;   ud->name = removePath(pth);   data = ud;   return ud;
  }


UnitDsc* UnitList::addDir(TCchar* path) {
String   pth = site.toRelative(path);
UnitKey  key;
UnitDsc* sf;
int      pos;

  if (pth.isEmpty() || filterFile(pth)) return 0;

  sf = findDir(pth); if (sf) return sf;

  sf = data.allocate();   sf->key.dir = true;   sf->key.path = pth;

  pos = pth.length() - 1;   if (pth[pos] == _T('\\')) pth = pth.substr(0, pos);

  sf->name = removePath(pth) + _T('\\');

  data = sf;   return sf;
  }


UnitDsc* UnitList::add(UnitDsc& uf) {
UnitDsc* sf = find(uf.key);

  if (!sf) return data = uf;    return sf;
  }


// Full or relative path to either a web or local file

UnitDsc* UnitList::update(TCchar* path) {
String   pth   = path;
bool     isWeb = pth.find(_T('/')) >= 0;
UnitDsc* ud;

  pth = site.toRelative(path);   if (pth.isEmpty() || filterFile(pth)) return 0;

  ud = find(pth);   if (!ud) return addFile(path);

  if (isWeb) site.getRmtAttr(pth, ud->size, ud->date);
  else       site.getAttr(   pth, ud->size, ud->date);

  return ud;
  }


// Full or relative path to either a web or local dir

UnitDsc* UnitList::updtDir(TCchar* path) {
String   pth = site.toRelative(path);
UnitDsc* ud;

  if (pth.isEmpty() || filterFile(path)) return 0;

  ud = findDir(pth);   if (ud) return ud;

  return addDir(path);
  }


UnitDsc* UnitList::update(UnitDsc& uf) {
UnitDsc* sf = find(uf.key);

  if (sf) {sf ->size = uf.size; sf->date = uf.date;   return sf;}
  else     return data = uf;
  }



static TCchar* filters[] = {_T("_notes\\"), _T("Junk\\"),   _T(".log"),   _T("tmp\\"),  _T(".vpj"),
                            _T(".vpw"),     _T(".vpwhist"), _T(".vtg"),   _T(".xml")
                            };

bool UnitList::filterFile(TCchar* path) {
String s = path;
int    i;
int    n;

  for (i = 0, n = noElements(filters); i < n; i++ ) if (s.find(filters[i]) >= 0) return true;

  return false;
  }


UnitDsc* UnitList::find(TCchar* filePath) {
UnitKey key;                                  // defaults to file obj

  key.path = site.toRelative(filePath);   return find(key);
  }


UnitDsc*  UnitList::findDir(TCchar* filePath) {
String  path = site.toRelative(filePath);
UnitKey key;

  key.dir = true;   key.path = getPath(path);   return find(key);
  }


void UnitList::delRcd(UnitDsc& uf) {
UnitListIter iter(*this);
UnitDsc* sf;

  for (sf = iter(); sf; sf = iter++) if (sf->key.path == uf.key.path) {iter.remove(); return;}
  }



bool UnitList::loadFromCSV(Archive& ar) {
CSVLex     lex(ar);
CSVtokCode code;
CSVtokCode code1;
int        i;
int        version = 0;
uint       t;

  clear();

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); break;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code == CommaToken && code1 == StringToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : if (tok.name != site.name) return false;
                break;

      case 1  : if (tok.name != site.url) {
                  notePad << _T("Web Name differs: ") << site.url << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;
      case 2  : if (tok.name != site.lclRoot()) {
                  notePad << _T("Path differs: ") << site.lclRoot() << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;

      case 3  : version = tok.name.stoi(t); break;

      default : notePad << _T("Whoops!") << nCrlf;
      }

    lex.accept_two_tokens();
    }

  return load(lex);
  }



bool UnitList::load(CSVLex& lex) {
int i;

  data.clear();

  for (i = 0; ; i++) {
    UnitDsc unitDsc;

    if (!unitDsc.load(lex)) return i > 0;

    data = unitDsc;
    }
  }



void UnitList::saveCSV(Archive& ar) {
CSVOut       csvOut(ar);
UnitListIter iter(*this);
UnitDsc* dsc;

  csvOut << site.name << Comma << site.url << Comma;
  csvOut << Version << Comma << vCrlf;

  for (dsc = iter(); dsc; dsc = iter++) dsc->save(csvOut);
  }




void UnitList::display(TCchar* title) {
UnitListIter iter(*this);
UnitDsc*     ud;

  notePad << nClrTabs << nSetRTab(32) << nSetRTab(43) << nSetTab(45) << nCrlf;

  notePad << nBeginLine << title;
  notePad << nTab << nData() << nEndLine << nCrlf << nCrlf;

  for (ud = iter(); ud; ud = iter++) ud->display();
  }



void UnitList::logSelected(TCchar* title) {
#if 0
UnitListIter iter(*this);
UnitDsc* dsc;

  if (!isLogging()) return;

  notePad << nClrTabs << nSetRTab(18) << nSetRTab(22) << nSetTab(24);
  notePad << nSetRTab(35) << nSetRTab(47) << nSetTab(49) << nCrlf;

  notePad << title << _T("   Log Selected -- Path: ") << root;
  notePad << _T(",   No of files: ") << nData() << nCrlf;
  notePad << nBeginLine << _T("Name") << nTab << _T("Chkd") << nTab << _T("Updt") << nTab << _T("Status");
  notePad << nTab << _T("Size") << nTab << _T("Date") << nTab << _T("Path") << nEndLine << nCrlf << nCrlf;

  for (dsc = iter(); dsc; dsc = iter++)
                                      if (dsc->check || dsc->status != NilOp || dsc->updated) dsc->log();
#endif
  }





///------------------

#if 0
bool UnitDsc::load(CSVLex& lex) {
CSVtokCode code;
CSVtokCode code1;
int        i;
uint       x;

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); return true;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code != StringToken || code1 != CommaToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : name = tok.name; break;

      case 1  : key.dir = 0; key.path = tok.name; break;

      case 2  : size = tok.name.stoi(x); break;

      case 3  : {ToDate lcldt(tok.name);  date = lcldt();} break;

      default : notePad << _T("Whoops!") << nCrlf;
      }
    lex.accept_two_tokens();
    }

  return false;
  }
#endif
#if 0
void UnitList::setCheck() {
UnitListIter iter(*this);
UnitDsc* dsc;

  for (dsc = iter(); dsc; dsc = iter++)
    if (dsc->status != NilOp)
      dsc->check = true;
  }


bool UnitList::updateFromPC() {
UnitList newDscs;
UnitListIter iter(newDscs);
UnitDsc* dsc;
UnitDsc* p;

  if (!newDscs.loadFromPC()) return false;

  checkForDel(newDscs);

  for (dsc = iter(); dsc; dsc = iter++) {
    UnitKey& key  = dsc->key;
    String       path = siteID.localRoot + key.path;

    p = find(key);   if (!p) {add(key);   continue;}

    if (!key.dir) p->addLclAttr(path);
    }

  return true;
  }
#endif
#if 0
void UnitList::checkForDel(UnitList& newDscs) {
UnitListIter iter(*this);
UnitDsc* dsc;

  for (dsc = iter(); dsc; dsc = iter++) {

    if (dsc->updated) {
      if (dsc->status == DelSts) {iter.remove();   continue;}

      if (dsc->status == DifPutSts) dsc->clrSts();
      }

    if (!newDscs.find(dsc->key)) iter.remove();
    }
  }
#endif
#if 0
void UnitList::update(UnitList& curDscs) {
UnitListIter iter(curDscs);
UnitDsc* dsc;
UnitDsc* p;

  root = curDscs.root;   rootLng = curDscs.rootLng;   loaded = curDscs.loaded;

  updateDel(curDscs);

  for (dsc = iter(); dsc; dsc = iter++) {

    if (dsc->status == DelSts && dsc->updated) {iter.remove(); continue;}

    p = find(dsc->key);

    if (p) *p = *dsc;
    else    p = add(*dsc);

    p->clrSts();    dsc->clrSts();
    }
  }

void UnitList::updateDel(UnitList& curDscs) {
UnitListIter iter(*this);
UnitDsc* dsc;
UnitDsc* p;

  for (dsc = iter(); dsc; dsc = iter++) {

    p = curDscs.find(dsc->key);

    if (p && p->status == DelSts && p->updated) iter.remove();
    }
  }


// Clear status for all entries that have been updated

void UnitList::clrSts() {
UnitListIter iter(curFileDscs);
UnitDsc* dsc;

  for (dsc = iter(); dsc; dsc = iter++) if (dsc->updated) {

    switch (dsc->status) {
      case DelSts   : iter.remove();   break;

      case GetSts   : dsc->clrSts();   break;

      case WebPutSts:
      case DifPutSts: dsc->clrSts();   break;

      default       : break;
      }
    }
  }
#endif
#if 0
bool UnitList::load(CSVLex& lex) {
int i;

  data.clear();

  for (i = 0; ; i++) {
    UnitDsc siteFile;

    if (!siteFile.load(lex)) return i > 0;

    data = siteFile;
    }
  }
#endif

