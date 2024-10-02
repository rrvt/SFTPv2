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
   if (!iniFile.read(sect, LocalPathKey, root, _T(""))) return false;

  fixLocalPath(root);   return true;
  }


bool LocalSite::save(TCchar* sect) {return iniFile.write(sect, LocalPathKey, root);}


bool LocalSite::createDir(TCchar* relPath) {
String path = fullDirPath(relPath);
String stk[16];
int    stkX = 0;
String right;

  while (!path.isEmpty() && path != root) {
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



bool LocalSite::loadTransport(TCchar* relPath)
  {return doc()->loadXfrBuffer(fullFilePath(relPath));}


bool LocalSite::storTransport(TCchar* relPath) {
bool rslt = doc()->storeXfrBuffer(fullFilePath(relPath));

  closeTransport();   return rslt;
  }


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


bool LocalSite::getPath() {
String path;

  iniFile.readString(GlobalSect, LocalPathStart, path);

  if (!getDirPathDlg(_T("Web Site"), path)) return false;

  root = fixLocalPath(path);

  iniFile.writeString(GlobalSect, LocalPathStart, root);   return true;
  }


void LocalSite::getAttr(TCchar* relPath, int& size, Date& date) {
String path = root + relPath;
FileIO lu;
CTime  time;

  if (lu.open(path, FileIO::Read)) {

    size = lu.getLength();

    if (!lu.getModifiedTime(time)) date.getToday();   date = (CTime) time;

    lu.close();
    }

  else {size = 1; date.getToday();}
  }



// Returns a relative local address

String&  LocalSite::toRelative(TCchar* fullPath) {
  path = fullPath;

  if (path.find(root) == 0) path = path.substr(root.length());

  return path;
  }



//-----------------------------

#if 1
#else
  if (createDirectory(path)) return true;

  for (rslt = rmvLastDir(path, right, _T('\\')), stkX = 0; rslt;
                                                        rslt = rmvLastDir(path, right, _T('\\'))) {
    stk[stkX++] = right;   if (createDirectory(path)) break;
    }
#endif

