// Unit Descriptor -- file path or directory path plus some attributes



#include "pch.h"
#include "UnitDsc.h"
#include "Filename.h"
#include "CSVLex.h"
#include "CSVOut.h"


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

      case 1  : key.dir = tok.name.stoi(x); break;

      case 2  : key.path = tok.name; break;

      case 3  : size = tok.name.stoi(x); break;

      case 4  : {ToDate lcldt(tok.name);  date = lcldt();} break;

      default : notePad << _T("Whoops!") << nCrlf;
      }

    lex.accept_two_tokens();
    }

  return false;
  }

#if 0
void UnitDsc::addLclAttr(TCchar* path) {
FileIO lu;
CTime  time;

  if (lu.open(path, FileIO::Read)) {

    size = lu.getLength();
    if (!lu.getModifiedTime(time)) time = CTime::GetCurrentTime();

    lu.close();
    }

  else {size = 1; time = CTime::GetCurrentTime();}

  date = time;
  }
#endif


void UnitDsc::save(CSVOut& csvOut) {

  csvOut << name     << Comma;
  csvOut << key.dir  << Comma;
  csvOut << key.path << Comma;
  csvOut << size     << Comma;
  csvOut << date     << Comma;
  csvOut << vCrlf;
  }


void UnitDsc::display() {
String d  = date;

  notePad << name << nTab << size << nTab << d << nTab << key.path << nCrlf;
  }


void UnitDsc::set(TCchar* relPath, bool dir, UnitOp op) {
String& path = key.path;

  normalize(relPath);   key.dir = dir;   unitOp = op;

#if 1
  name = dir ? path.substr(0, path.length()-1) : path;   name = removePath(name);
#else
  if (!dir)                                                         name = removePath(path);
  else {String pth = path;   pth = pth.substr(0, pth.length()-1);   name = removePath(pth);}
#endif
  }


String& UnitDsc::normalize(TCchar* relPath) {
String& path = key.path;
int     n    = path.length();
int     i;
int     pos;
String  prefix;

  path = relPath;   n = path.length();

  for (i = 0; i < n; i++) if (path[i] == _T('/')) path[i] = _T('\\');

  pos = path.find(_T("\\\\"));

  if (pos >= 0) {prefix = path.substr(0, pos+1);   path = prefix + path.substr(pos+2);}

  return path;
  }


bool UnitDsc::operator<  (UnitKey& s) {return s >  key;}
bool UnitDsc::operator<= (UnitKey& s) {return s >= key;}             // Required for Binary Search
bool UnitDsc::operator== (UnitKey& s) {return s == key;}             // Required for Binary Search
bool UnitDsc::operator!= (UnitKey& s) {return s != key;}
bool UnitDsc::operator>  (UnitKey& s) {return s <  key;}
bool UnitDsc::operator>= (UnitKey& s) {return s <= key;}


bool UnitKey::operator== (UnitKey& k) {return  dir == k.dir && _tcsicmp(path, k.path) == 0;}
bool UnitKey::operator!= (UnitKey& k) {return  dir != k.dir || _tcsicmp(path, k.path) != 0;}

bool UnitKey::operator<  (UnitKey& k)
                          {return  dir > k.dir || (dir == k.dir && _tcsicmp(path, k.path) <  0);}
bool UnitKey::operator<= (UnitKey& k)
                          {return (dir > k.dir || (dir == k.dir && _tcsicmp(path, k.path) <= 0));}

bool UnitKey::operator>  (UnitKey& k)
                          {return  dir < k.dir || (dir == k.dir && _tcsicmp(path, k.path) >  0);}
bool UnitKey::operator>= (UnitKey& k)
                          {return (dir < k.dir || (dir == k.dir && _tcsicmp(path, k.path) >= 0));}




//--------------

#if 0

                          bool UnitDsc::put(int& noFiles) {
                          #if 0
                          String   lclPath;
                          String   webPath;
                          UnitDsc* unitDsc;

                            if (key.dir) return createWebDir(noFiles);

                            unitDsc = curFileDscs.findDir(key.path);

                            if (unitDsc && unitDsc->unitOp != NilOp && !unitDsc->createWebDir(noFiles)) return false;

                            lclPath = site.localRoot  + key.path;

                            if (!sftpSSL.getLocalFile(lclPath)) return false;

                            webPath = site.root + key.path;

                            if (!sftpSSL.stor(toRemotePath(webPath))) return false;

                            sendStepPrgBar();   noFiles++;   updated = true;   return true;        //webFiles.modified();
                          #endif
                            }


                          bool UnitDsc::createWebDir(int& noFiles) {
                          #if 0
                          String       right = key.path;
                          String       left;
                          int          pos;
                          UnitKey  key;
                          UnitDsc* dsc;
                          String       fullPath;

                            for (pos = right.find(_T('\\')); pos >= 0; pos = right.find(_T('\\'))) {

                              left += right.substr(0, pos+1);   right = right.substr(pos+1);

                              key.dir = true;   key.path = left;   dsc = curFileDscs.find(key);

                              if (!dsc || (dsc->status != NilOp && !dsc->updated)) {
                                fullPath = siteID.remoteRoot + left;   if (!sftpSSL.mkd(toRemotePath(fullPath))) return false;

                                sendStepPrgBar();   noFiles++;   if (dsc) dsc->updated = true;
                                }
                              }
                          #endif
                            return true;
                            }


                          bool UnitDsc::get(int& noFiles) {
                          #if 0
                          String webPath;
                          String lclPath;
                          String rslt;

                            if (key.dir) return createLocalDir(noFiles);

                            webPath = siteID.remoteRoot + key.path;

                            if (!sftpSSL.retr(toRemotePath(webPath))) return false;

                            lclPath = siteID.localRoot + key.path;

                            if (!doc()->saveData(StoreSrc, toLocalPath(lclPath))) return false;

                            sendStepPrgBar();   noFiles++;   updated = true;   addLclAttr(lclPath);

                            sftpSSL.noop(rslt);
                          #endif
                            return true;
                            }


                          bool UnitDsc::createLocalDir(int& noFiles) {
                          #if 0
                          String       right = key.path;
                          String       left;
                          int          pos;
                          UnitKey  key;
                          UnitDsc* dsc;
                          String       fullPath;

                            for (pos = right.find(_T('\\')); pos >= 0; pos = right.find(_T('\\'))) {

                              left += right.substr(0, pos+1);   right = right.substr(pos+1);

                              key.dir = true;   key.path = left;   dsc = curFileDscs.find(key);

                              if (!dsc || (dsc->status != NilOp && !dsc->updated)) {
                                fullPath = siteID.localRoot + left;

                                if (!CreateDirectory(fullPath, 0)) return false;

                                sendStepPrgBar();   noFiles++;   if (dsc) dsc->updated = true;
                                }
                              }
                          #endif
                            return true;
                            }


                          bool UnitDsc::del(int& noFiles) {
                          #if 0
                          String webPath;

                            if (key.dir) return removeDir(noFiles);

                            webPath = siteID.remoteRoot + key.path;   webPath = toRemotePath(webPath);

                            if (!sftpSSL.del(webPath)) return false;

                            sendStepPrgBar();   noFiles++;   updated = true;
                          #endif
                            return true;
                            }


                          bool UnitDsc::removeDir(int& noFiles) {
                          #if 0
                          String       prefix = key.path;
                          UnitListIter iter(curFileDscs);
                          UnitDsc* dsc;
                          DirList      dl;
                          DLIter       dlIter(dl);
                          DirItem*     item;

                            for (dsc = iter(); dsc; dsc = iter++)
                                                         {String& path = dsc->key.path;   if (path.find(prefix) == 0) dl.add(dsc);}

                            for (item = dlIter(); item; item = dlIter++) {
                              UnitDsc* dsc = item->dsc;

                              if (!dsc->key.dir) {if (!dsc->del(noFiles)) return false;   continue;}

                          //  notePad << item->level << nTab << dsc->key.path << nCrlf;

                              String webPath = siteID.remoteRoot + dsc->key.path;   webPath = toRemotePath(webPath);

                              if (!sftpSSL.rmd(webPath)) return false;

                              sendStepPrgBar();   noFiles++;   dsc->updated = true;
                              }
                            return dl.nData() > 0;
                          #endif
                            return true;
                            }


                          void UnitDsc::log() {
                          #if 0
                          String d    = date;
                          String chk  = check ? _T("X  ") : _T("_  ");
                          String sts;
                          String updt = updated ? _T("Updt") : _T("_  ");

                            switch (status) {
                              case NilOp   : sts = _T("NilOp");    break;
                              case WebPutSts: sts = _T("WebPutSts"); break;
                              case DifPutSts: sts = _T("DifPutSts"); break;
                              case GetSts   : sts = _T("GetSts");    break;
                              case DelSts   : sts = _T("DelSts");    break;
                              case OthSts   : sts = _T("OthSts");    break;
                              default       : sts = _T("Unknown");   break;
                              }

                            notePad << name << nTab << chk << nTab << updt << nTab << sts;
                            notePad << nTab << size << nTab << d << nTab << key.path << nCrlf;
                          #endif
                            }
#endif
