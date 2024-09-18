


#include "pch.h"
#include "PickTransaction.h"
#include "MessageBox.h"
#include "NotePad.h"
#include "SftpSSLv2.h"
#include "UnitList.h"

// First Principal  -- The local directory may be added to but never reduced.
// Second Prinicpal -- The remote directory may only be changed with user confirmation, i.e. a
//                     check box in a dialog.
//
// There are 4 transactions on the local directory and 4 transactions on the remote web directory:
//   o  GetOp -- copy file from remote directory to local directory
//   o  delete file from local directory -- Not done by this app
//   o  MkLclDirOp -- create a new directory in the local directory tree somewhere (may require
//      more than one directory creation)
//   o  delete directory from local directory (may require deleting all files in the directory
//      first) -- Not done by this app

//   o  PutOp -- copy file from local directory to remote directory
//   o  DelOp -- delete file from remote directory
//   o  MkRmtDirOp -- create a new directory in the remote directory tree somewhere (may require
//      more than one directory creation)
//   o  DelOp -- delete directory from remote directory (may require deleting all files in the
//      directory first)

// Operations take place on entries in the updateList.  The updateList entries are added by
// various comparison operations.  Herein we will place an entry in the update list and then
// return.  Since this function is called from the update command it will then proceed to perform
// the update.


static TCchar* Pick    = _T("D:\\Webs\\swdeTest\\SacrilegiousProgGuide\\Sub\\Beta\\NewOne.htm");
static TCchar* GetPick = _T("SacrilegiousProgGuide\\Sub\\Beta\\NewOne.htm");
static TCchar* DelPick = _T("SacrilegiousProgGuide\\Sub\\Beta\\");

void PickTransaction::operator() () {
UnitDsc* dsc = localDirList.find(Pick);
UnitDsc* updateDsc;
UnitDsc  ud;
  if (dsc) {
    updateDsc = updateList.add(*dsc);   updateDsc->unitOp = PutOp;

    ud.key.path = dsc->key.path;   ud.key.dir = false;   ud.name = _T("NewOne.htm");
    ud.unitOp = DelOp;    updateList.add(ud);

    ud.key.path = DelPick;   ud.key.dir = true;   ud.name = _T("Beta");   ud.unitOp = DelDirOp;
    updateList.add(ud);

    ud.key.path = GetPick;   ud.key.dir = false;   ud.name = _T("NewOne.htm");
    ud.unitOp = GetOp;    updateDsc = updateList.add(ud);
    }
  }

