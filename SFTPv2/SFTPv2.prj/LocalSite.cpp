// Local Site details


#include "pch.h"
#include "LocalSite.h"
#include "AppUtilities.h"
#include "Date.h"
#include "FileIO.h"
#include "FileName.h"
#include "GetPathDlg.h"
#include "IniFile.h"
#include "SFTPv2Doc.h"
#include "Site.h"
#include "UnitList.h"


static TCchar* LocalPathStart = _T("LocalPathStart");
static TCchar* LocalPathKey   = _T("Path");


bool LocalSite::load(TCchar* sect) {
String s;

  if (!iniFile.read(sect, LocalPathKey, s, _T(""))) return false;

  pathXform.set(s);   return true;
  }


bool LocalSite::save(TCchar* sect) {return iniFile.write(sect, LocalPathKey, pathXform.get());}


bool LocalSite::loadTransport(TCchar* relPath)
                                            {return doc()->loadXfrBuffer(fullFilePath(relPath));}


bool LocalSite::storTransport(TCchar* relPath) {
String path = fullFilePath(relPath);

  if (!createDir(relPath)) return false;

  return doc()->storeXfrBuffer(path);
  }


bool LocalSite::createDir(TCchar* relPath) {
String path = fullDirPath(relPath);
String stk[16];
int    stkX = 0;
String right;

  while (!path.isEmpty()) {
    if (createDirectory(path) || !rmvLastDir(path, right, _T('\\'))) break;

    stk[stkX++] = right;
    }

  for (stkX--; stkX >= 0; stkX--) {path += stk[stkX];   if (!createDirectory(path)) return false;}

  return true;
  }


bool LocalSite::createDirectory(TCchar* fulPath) {
UnitDsc ud;

  ud.set(toRelative(fulPath), true, NilOp);   baseLineList.add(ud);   localDirList.add(ud);

  return CreateDirectory(fulPath, 0) || GetLastError() == ERROR_ALREADY_EXISTS;
  }


String& LocalSite::fullDirPath( TCchar* relPath)
                                  {return pathXform.toLocal(pathXform.toFull(::getPath(relPath)));}
String& LocalSite::fullFilePath(TCchar* relPath)
                                  {return pathXform.toLocal(pathXform.toFull(relPath));}



bool LocalSite::getPath() {
String path;

  iniFile.readString(GlobalSect, LocalPathStart, path);

  if (!getDirPathDlg(_T("Web Site"), path)) return false;

  pathXform.set(path);

  iniFile.writeString(GlobalSect, LocalPathStart, pathXform.get());   return true;
  }


void LocalSite::getAttr(TCchar* relPath, int& size, Date& date) {
String path = fullFilePath(relPath);
FileIO lu;
CTime  time;

  if (lu.open(path, FileIO::Read)) {

    size = lu.getLength();

    if (lu.getModifiedTime(time)) date = time;
    else                          date.getToday();

    lu.close();
    }

  else {size = 1; date.getToday();}
  }





//-----------------------------

#if 0
// Returns a relative local address

String&  LocalSite::toRelative(TCchar* fullPath) {
  path = fullPath;

  if (path.find(root) == 0) path = path.substr(root.length());

  return path;
  }
#endif

#if 1
#else
  if (createDirectory(path)) return true;

  for (rslt = rmvLastDir(path, right, _T('\\')), stkX = 0; rslt;
                                                        rslt = rmvLastDir(path, right, _T('\\'))) {
    stk[stkX++] = right;   if (createDirectory(path)) break;
    }
#endif
#if 1
#else
String& LocalSite::fullDirPath(TCchar* relPath) {return fullFilePath(::getPath(relPath));}


String& LocalSite::fullFilePath(TCchar* relPath) {path = root + relPath;   return fixSeparators();}


String& LocalSite::fixSeparators() {
int    n = path.length();
int    i;
int    pos;
String prefix;

  for (i = 0; i < n; i++) if (path[i] == _T('/')) path[i] = _T('\\');

  pos = path.find(_T("\\\\"));

  if (pos >= 0) {prefix = path.substr(0, pos+1);   path = prefix + path.substr(pos+2);}

  return path;
  }
#endif

