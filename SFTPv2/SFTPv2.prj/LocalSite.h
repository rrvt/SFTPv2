// Local Site details


#pragma once
#include "AppUtilities.h"
#include "RemoteSite.h"

class Date;


class LocalSite : public RemoteSite {

String root;
String path;                      // Used to create a full path

public:
          LocalSite() { }
         ~LocalSite() { }

  void    clear()               {root.clear();}
  void    setRoot(TCchar* path) {root = toLocal(path);}
  String& getRoot()             {return root;}
  String& toLocal(TCchar* path) {this->path = path;   return fixSeparators();}

  String& fullDirPath( TCchar* relPath);
  String& fullFilePath(TCchar* relPath);

  bool    load(TCchar* sect);
  bool    save(TCchar* sect);

  bool    loadTransport(TCchar* relPath);
  bool    storTransport(TCchar* relPath);

  bool    createDir(TCchar* relPath);

  bool    getPath();

  String& toRelative(TCchar* fullPath);
  void    getAttr(TCchar* relPath, int& size, Date& date);

private:

  bool    createDirectory(TCchar* path);
  String& fixSeparators();

  friend class Site;
  };

