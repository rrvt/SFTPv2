// Site Details


#pragma once
#include "LocalSite.h"

class UnitList;
class UnitDsc;


extern TCchar* GlobalSect;
extern TCchar* LastSiteKey;


class Site : public LocalSite {

//static volatile UnitDsc* lastDsc;

public:

String name;

  Site() { }
 ~Site() { }

  void    clear() {name.clear();   LocalSite::clear();   RemoteSite::clear();}
  String& lclRoot() {return LocalSite::getRoot();}
  String& rmtRoot() {return RemoteSite::getRoot();}
  void    setRmtRoot(TCchar* path) {RemoteSite::setRoot(path);}

  bool    setLastSite();

  bool    loadData(String& sect);            // Load from iniFile
  bool    saveData();                        // save to iniFile

  bool    edit();

  void    update();
  LRESULT finUpdate(WPARAM wparam, LPARAM lParam);
  void    display(UnitList& ul);

  String  dataFileName();

  String  toFull(TCchar* relPath, bool isWeb);          // Returns a full path from a local
  String& normalizePath(TCchar* path) {return LocalSite::toLocal(path);}

private:

  bool    doXfer(UnitList& ul);
  void    performOp(UnitDsc* dsc);                      // Perform operation
  void    put(UnitDsc& ud);                             // Put local file to remote host
  void    get(UnitDsc& ud);                             // Get host and store in local file
  void    del(UnitDsc& ud);                             // Del file in remote host
  void    delDir(UnitDsc& ud);                          // Del dir in remote host

  friend UINT updateThrd(   void* param);               // Thread for processing an update
  };


extern Site site;
