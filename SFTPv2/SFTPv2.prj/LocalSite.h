// Local Site details


#pragma once
#include "AppUtilities.h"

class Date;


class LocalSite {

String root;
String path;
public:


          LocalSite() { }
         ~LocalSite() { }

  void    clear()                  {root.clear();}
  void    set(TCchar* path)        {root = toLocal(path);}
  String& getRoot()                {return root;}
  String& getPath(TCchar* relPath) {path = root + relPath; path = toLocal(path); return path;}

  bool    load(TCchar* sect);
  bool    save(TCchar* sect);

//  bool    loadTransport(TCchar* relPath);
//  bool    storTransport(TCchar* relPath);

  bool    createDir(TCchar* relPath);

  bool    getPath();

  void    getAttr(TCchar* relPath, int& size, Date& date);

private:

  bool    createDirectory(TCchar* path);

  friend class Site;
  };

