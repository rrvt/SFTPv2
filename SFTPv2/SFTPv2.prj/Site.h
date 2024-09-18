// Site Details


#pragma once
#include "LocalSite.h"
#include "RemoteSite.h"

class UnitList;
class UnitDsc;


extern TCchar* GlobalSect;
extern TCchar* LastSiteKey;


class Site : public LocalSite, public RemoteSite {

static volatile UnitDsc* lastDsc;

public:

String name;

  Site() { }
 ~Site() { }

  void    clear() {name.clear();   LocalSite::clear();   RemoteSite::clear();}
  String& lclRoot() {return LocalSite::getRoot();}
  String& rmtRoot() {return RemoteSite::getRoot();}
  void    setRmtRoot(TCchar* path) {RemoteSite::set(path);}

  bool    setLastSite();

  bool    loadData(String& sect);            // Load from iniFile
  bool    saveData();                        // save to iniFile

  bool    edit();

  void    update();
  bool    doXfer(UnitList& ul);

  LRESULT finUpdate(WPARAM wparam, LPARAM lParam);
  void    display();

  String  dataFileName();

  String  toRelative(TCchar* path);                     // Returns a relative local address
  String  toFull(TCchar* relPath, bool isWeb);          // Returns a full path from a local

private:

  void    performOp(UnitDsc* dsc);                      // Perform operation
  bool    put(UnitDsc& ud);                             // Put local file to remote host
  bool    get(UnitDsc& ud);                             // Get host and store in local file
  bool    del(UnitDsc& ud);                             // Del file in remote host
  bool    delDir(UnitDsc& ud);                          // Del dir in remote host
  };


extern Site site;
