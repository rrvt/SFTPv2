// Local Site details


#pragma once
#include "AppUtilities.h"
#include "PathXform.h"
#include "RemoteSite.h"

class Date;


class LocalSite : public RemoteSite {

//String root;
PathXform pathXform;

public:
          LocalSite() { }
         ~LocalSite() { }

  void    clear()                      {pathXform.clear();}
  void    setRoot(TCchar* path)        {pathXform.set(path);}
  String& getRoot()                    {return pathXform.get();}
  String& toLocal(TCchar* path)        {return pathXform.toLocal(path);}
  String& toRelative(TCchar* fullPath) {return pathXform.toRelative(fullPath);}
  String& fullDirPath( TCchar* relPath);
  String& fullFilePath(TCchar* relPath);

  bool    load(TCchar* sect);               // Load/Save from/to App's IniFile
  bool    save(TCchar* sect);

  bool    loadTransport(TCchar* relPath);
  bool    storTransport(TCchar* relPath);

  bool    getPath();

  void    getAttr(TCchar* relPath, int& size, Date& date);

private:

  bool    createDir(TCchar* relPath);
  bool    createDirectory(TCchar* path);

  friend class Site;
  };




