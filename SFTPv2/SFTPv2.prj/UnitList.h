// Site Files -- A list of all the site files (filtered) in the local and remote site tree
// All paths are represented as local paths, beware when using the path with the web site


#pragma once
#include "ExpandableP.h"
#include "IterT.h"
#include "UnitDsc.h"


class UnitList;

typedef DatumPtrT<UnitDsc, UnitKey> UnitDscP;
typedef IterT<UnitList, UnitDsc>    UnitListIter;           // Iterator Declaration


class UnitList {                                            // List of directories and files

bool   loaded;

ExpandableP<UnitDsc, UnitKey, UnitDscP, 2> data;            // List of all files in web site
                                                            // (local, remote, previous, etc.)
public:
                UnitList() : loaded(false) { }
                UnitList(UnitList& ul) {copy(ul);}
               ~UnitList() {clear();}

  void          clear() {data.clear();}
  void          clrSts();

  bool          isEmpty() {return nData() == 0;}

  UnitList&     operator= (UnitList& ul) {copy(ul);   return *this;}

  bool          loadFromPC();
  UnitDsc*      addFile(TCchar* path);      // full path to either a web file or local file
  UnitDsc*      addDir( TCchar* path);
  UnitDsc*      add(    UnitKey& key);      // Key contains local relative path (rel to localRoot)

  UnitDsc*      add(   UnitDsc& ud);        // Adds copy of ud if not already in list
                                            // Updates first determine if unit is in the list
                                            // adds if not.
  UnitDsc*      update(TCchar*  path);      // Full or relative path to either a web or local file
  UnitDsc*      updtDir(TCchar* path);      // Full or relative path to either a web or local dir
  UnitDsc*      update(UnitDsc& ud);        // Adds ud if not already in list

  void          delRcd(UnitDsc& ud);

  bool          loadFromCSV(Archive& ar);
  void          saveCSV(    Archive& ar);

  UnitDsc*      find(   TCchar* filePath);      // Find file, need full path or relative to root
  UnitDsc*      findDir(TCchar* path);          // Find dir, need full path or relative to root

  void          display(TCchar* title);

  void          logSelected(TCchar* title);

  int           nData() {return data.end();}    // returns number of data items in array

private:

  UnitDsc*      find(UnitKey& key) {return data.bSearch(key);}

  void          copy(UnitList& ul);

  void          loadOneDir(TCchar* path);

  bool          load(CSVLex& lex);

  bool          filterFile(TCchar* path);

  void          checkForDel(UnitList& newDscs);

  UnitDsc*      datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  void          removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename UnitListIter;
  };


extern UnitList baseLineList;             // Representation of WebDirList after an update
extern UnitList localDirList;             // List of files and directories on local PC
extern UnitList webDirList;               // List of files and directories on the remote host
extern UnitList updateList;               // List of files and directories to update next


//  void      startFromWeb(TCchar* path);
//  void      fromWeb(TCchar* path);

// returns either a pointer to data (or datum) at index i in array or zero
//extern UnitList updateFileDscs;
//  void          setCheck();

//UnitDsc* bSearchxx(UnitKey& key);   // Debugging
//bool          load(CSVLex& lex);

//bool          updateFromPC();
//  void          update(UnitList& curDscs);
//  void          updateDel(UnitList& curDscs);

