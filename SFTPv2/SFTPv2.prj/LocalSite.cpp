// Local Site details


#include "pch.h"
#include "LocalSite.h"
#include "AppUtilities.h"
#include "Date.h"
#include "FileIO.h"
#include "FileName.h"
#include "GetPathDlg.h"
#include "IniFile.h"
#include "Site.h"


static TCchar* LocalPathStart = _T("LocalPathStart");
static TCchar* LocalPathKey   = _T("Path");


bool LocalSite::load(TCchar* sect) {
   if (!iniFile.read(sect, LocalPathKey, root, _T(""))) return false;

  fixLocalPath(root);   return true;
  }


bool LocalSite::save(TCchar* sect) {return iniFile.write(sect, LocalPathKey, root);}


bool LocalSite::createDir(TCchar* relPath) {
String path = ::getPath(relPath);   path = getPath(path);
bool   rslt;
String stk[16];
int    stkX;
String right;

  if (createDirectory(path)) return true;

  for (rslt = rmvLastDir(path, right, _T('\\')), stkX = 0; rslt;
                                                        rslt = rmvLastDir(path, right, _T('\\'))) {
    stk[stkX++] = right;   if (createDirectory(path)) break;
    }

  for (stkX--; stkX >= 0; stkX--)
    {path += stk[stkX];   if (!createDirectory(path)) return false;}

  return true;
  }


bool LocalSite::createDirectory(TCchar* path)
                      {return CreateDirectory(path, 0) || GetLastError() == ERROR_ALREADY_EXISTS;}


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

