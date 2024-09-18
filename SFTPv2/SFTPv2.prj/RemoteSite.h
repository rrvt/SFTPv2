// Remote Site details


#pragma once
#include "AppUtilities.h"
#include "CNG.h"

class Date;


class RemoteSite {

bool   loggedIn;
String root;

public:

String url;

          RemoteSite() : loggedIn(false) { }
         ~RemoteSite() { }

  void    clear() {loggedIn = false;   root.clear();   url.clear();}
  void    set(TCchar* path) {root = toLocal(path);}
  String& getRoot() {return root;}

  bool    load(TCchar* sect);
  bool    save(TCchar* sect);

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

private:

  bool    isValid();

  friend class Site;
  };

