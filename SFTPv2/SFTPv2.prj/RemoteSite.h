// Remote Site details


#pragma once
#include "AppUtilities.h"
#include "CNG.h"
#include "FileName.h"
#include "PathXform.h"
#include "SftpSSLv2.h"

class Date;
class UnitList;
class UnitDsc;


class RemoteSite {

bool      loggedIn;
PathXform rmtXform;

public:

String url;

          RemoteSite() : loggedIn(false) { }
         ~RemoteSite() { }

  void    clear() {loggedIn = false;   rmtXform.clear();   url.clear();}
  void    setRoot(TCchar* path)         {rmtXform.set(rmtXform.toLocal(path));}
  String& getRoot()                     {return rmtXform.toRemote(rmtXform.get());}
  String& toRelative(TCchar* fullPath)  {return rmtXform.toRelative(rmtXform.toLocal(fullPath));}
  String& fullFilePath(TCchar* relPath) {return rmtXform.toRemote(rmtXform.toFull(relPath));}
  String& fullDirPath( TCchar* relPath)
                                   {return rmtXform.toRemote(rmtXform.toFull(::getPath(relPath)));}

  bool    load(TCchar* sect);                   // Load/Save from/to App's IniFile
  bool    save(TCchar* sect);

  bool    login();                              // Log into the web host with url/name/password
  void    logout();                             // Log out of web host

  bool    get(TCchar* relPath);                 // Get file from web host, copy to PC
  bool    put(TCchar* relPath);                 // Get file from PC, copy to web host

  void    compSites();
  LRESULT onCompSites(WPARAM wparam, LPARAM lParam);
  void    getRmtSite();
  LRESULT onDspRmtSite(WPARAM wparam, LPARAM lParam);

  void    getRmtAttr(TCchar* relPath, int& size, Date& date);

private:

  bool    delDir(TCchar* relPath);
  bool    del(TCchar* relPath);

  bool    openTransport(SftpIO io, TCchar* webPath) {return sftpSSL.openTransport(io, webPath);}
  void    closeTransport() {                                sftpSSL.closeTransport();}

  bool    createDir(TCchar* path);

  bool    doRmtDir(TCchar* path, UnitList& ul);
  bool    parse(String& line, TCchar* path, UnitDsc& item);

  bool    isValid();

  friend UINT getWebDirThrd(void* param);
  friend class Site;
  };



//------------------

//  bool    loadTransport(TCchar* relPath);
//  bool    storTransport(TCchar* relPath);
//  bool    readTransport();                                // Read from web host into store
//  bool    writeTransport(TCchar* relPath);                // Write from store into web host

