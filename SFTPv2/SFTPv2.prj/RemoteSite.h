// Remote Site details


#pragma once
#include "AppUtilities.h"
#include "CNG.h"

class Date;
class UnitList;
class UnitDsc;


class RemoteSite {

bool   loggedIn;
String root;
String path;                      // Used to create a full path

public:

String url;

          RemoteSite() : loggedIn(false) { }
         ~RemoteSite() { }

  void    clear() {loggedIn = false;   root.clear();   url.clear();}
  void    setRoot(TCchar* path);
  String& getRoot() {return root;}

  String& fullFilePath(TCchar* relPath);
  String& fullDirPath( TCchar* relPath);

  bool    load(TCchar* sect);
  bool    save(TCchar* sect);

  void    getRmtDir();
  LRESULT finRmtDir(WPARAM wparam, LPARAM lParam);

  bool    loadTransport(TCchar* relPath);
  bool    storTransport(TCchar* relPath);

  bool    login();
  void    logout();

  bool    createDir(TCchar* path);

  void    getRmtAttr(TCchar* relPath, int& size, Date& date);

  void    closeTransport();

protected:

  String  ensureSite(TCchar* name);
  bool    getNmPswd(String& name, String& pswd);
  bool    delDir(TCchar* relPath);
  bool    del(TCchar* relPath);
  String& fixSeparators(TCchar* path) {this->path = path; return fixSeparators();}
  String& fixSeparators();

private:

  bool    doRmtDir(TCchar* path, UnitList& ul);
  bool    parse(String& line, TCchar* path, UnitDsc& item);
  String& toRelative(TCchar* fullPath);

  bool    isValid();

  friend UINT getWebDirThrd(void* param);
  friend class Site;
  };

