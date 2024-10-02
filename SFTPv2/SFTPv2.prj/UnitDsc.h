// Unit Descriptor -- file path or directory path plus some attributes


#pragma once
#include "Date.h"


class CSVLex;
class CSVOut;
class Archive;                        // MkLclDirOp,   MkRmtDirOp,
                                      // MkLclDirdone, MkRmtDirDone,

enum   UnitOp  {NilOp,  PutOp,   GetOp,   DelOp,   DelDirOp,
                        PutDone, GetDone, DelDone, DelDirDone};

class UnitKey {
public:
bool   dir;
String path;

           UnitKey() : dir(false) { }
           UnitKey(TCchar* pth)  {dir = false; path = pth;}
           UnitKey(String& s)    {dir = false; path = s;}
           UnitKey(UnitKey& key) {copy(key);}

  void     clear() {dir = false; path.clear();}

  UnitKey& operator- (String& s)   {dir = false;   path = s;    return *this;}
  UnitKey& operator- (TCchar* pth) {dir = false;   path = pth;  return *this;}

  UnitKey& operator= (UnitKey& key) {copy(key); return *this;}

  bool     operator== (UnitKey& k);
  bool     operator!= (UnitKey& k);
  bool     operator<  (UnitKey& k);
  bool     operator<= (UnitKey& k);

  bool     operator>  (UnitKey& k);
  bool     operator>= (UnitKey& k);

private:

  void     copy(UnitKey& key) {dir = key.dir;   path = key.path;}
  };



class UnitDsc {
public:

UnitKey key;
String  name;                   // Name of file, <name>.<ext>
int     size;
Date    date;

UnitOp  unitOp;

           UnitDsc() : key(), size(0), date(Date::MinDate), unitOp(NilOp) { }
           UnitDsc(UnitDsc& siteFile) {copy(siteFile);}
          ~UnitDsc() { }

 void      clrOp() {unitOp = NilOp;}

  UnitDsc& operator= (UnitDsc& siteFile) {copy(siteFile); return *this;}

  void     set(TCchar* relPath, bool dir, UnitOp op);
  String&  normalize(TCchar* relPath);

  bool     load(CSVLex& lex);
  void     save(CSVOut& csvOut);
  void     display();

  // Allows sorted data

  bool     operator== (UnitDsc& dsc) {return key == dsc.key;}
  bool     operator!= (UnitDsc& dsc) {return key != dsc.key;}

  bool     operator>  (UnitDsc& dsc) {return key >  dsc.key;}
  bool     operator>= (UnitDsc& dsc) {return key >= dsc.key;}
  bool     operator<  (UnitDsc& dsc) {return key <  dsc.key;}
  bool     operator<= (UnitDsc& dsc) {return key <= dsc.key;}

  bool     operator<  (UnitKey& s);             // Required for Binary Search
  bool     operator<= (UnitKey& s);             // Required for Binary Search
  bool     operator== (UnitKey& s);
  bool     operator!= (UnitKey& s);
  bool     operator>  (UnitKey& s);
  bool     operator>= (UnitKey& s);

private:

  void copy(UnitDsc& ud) {
    name  = ud.name;    key    = ud.key;      size  = ud.size;
    date  = ud.date;    unitOp = ud.unitOp;
    }
  };


//-------------------

//bool   load(CSVLex& lex);

//  void   addLclAttr(TCchar* path);
#if 0
  bool   put(           int& noFiles);
  bool   createWebDir(  int& noFiles);
  bool   get(           int& noFiles);
  bool   createLocalDir(int& noFiles);
  bool   del(           int& noFiles);
  bool   removeDir(     int& noFiles);

  void   log();
#endif

