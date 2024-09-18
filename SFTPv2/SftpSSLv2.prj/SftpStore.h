// Store for data received from server


#pragma once
#include "ExpandableP.h"
#include "IterT.h"
#include "SftpBlock.h"
#include "SftpUtilities.h"



class SftpStore;
typedef DatumPtrT<SftpBlock, int>   SftpBlkP;     // Usually defined just before iterator
typedef IterT<SftpStore, SftpBlock> SftpStrIter;  // Iterator over array of Datums


class SftpStore {
protected:

SSLFileType                              fileType;
ExpandableP<SftpBlock, int, SftpBlkP, 2> data;

public:
String firstResp;
String lastResp;
Tchar  lastCh;


  SftpStore() : fileType(NilFlTyp) { }
 ~SftpStore() {clear();}

  void clear() {data.clear();   fileType = NilFlTyp;   clearLast();}

  SftpStore& operator -= (SftpStore& store);

  void setType(SSLFileType typ) {fileType = typ;}

  int  nData() {return data.end();}     // returns number of data items in array

protected:

  void clearLast() {firstResp.clear(); lastResp.clear(); lastCh = 0;}

  bool find(int code);
  bool find(Tchar ch);
  void getResp(String& s);

private:

  // returns either a pointer to data (or datum) at index i in array or zero

  SftpBlock* datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  void  removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename SftpStrIter;
//  friend class    SftpTransport;
  };

