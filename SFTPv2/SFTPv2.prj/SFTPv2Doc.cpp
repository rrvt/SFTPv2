 // SFTPv2Doc.cpp : implementation of the SFTPv2Doc class


#include "pch.h"
#include "SFTPv2Doc.h"
#include "ClipLine.h"
#include "filename.h"
#include "IniFile.h"
#include "MessageBox.h"
#include "NotePad.h"
#include "Printer.h"
#include "Resource.h"
#include "SftpSSLv2.h"
#include "SFTPv2View.h"
#include "UnitList.h"
#include "UpdateDlg.h"
#include "workerThrd.h"

//#include "PickTransaction.h"            // *** Debug ***

static TCchar* MyPassword = _T("SFTPv2App");


// SFTPv2Doc

IMPLEMENT_DYNCREATE(SFTPv2Doc, CDoc)

BEGIN_MESSAGE_MAP(SFTPv2Doc, CDoc)

  ON_COMMAND(ID_NewSite,          &onNewSite)
  ON_COMMAND(ID_PickSite,         &onPickSite)
  ON_COMMAND(ID_EditSite,         &onEditSite)
  ON_COMMAND(ID_CompSites,        &onCompSites)
  ON_COMMAND(ID_CompPrev,         &onCompPrevious)

  ON_COMMAND(ID_Update,           &onUpdate)
  ON_COMMAND(ID_ViewDetails,      &onViewDetails)

  ON_COMMAND(ID_TBSaveMenu,       &onSaveFile)
  ON_COMMAND(ID_SaveFile,         &onSaveFile)
  ON_COMMAND(ID_SaveNotePad,      &onSaveNotePad)

  ON_COMMAND(ID_EDIT_COPY,        &onEditCopy)

END_MESSAGE_MAP()


// SFTPv2Doc construction/destruction

SFTPv2Doc::SFTPv2Doc() noexcept : dataSource(NotePadSrc), cngBlk(0) { }

SFTPv2Doc::~SFTPv2Doc() { }


// Create a new site

void SFTPv2Doc::onNewSite() {                                   // XXX
String title;

  if (isLocked()) return;

  notePad.clear();    site.clear();

  if (site.edit())
            {site.logout();   site.login();   theApp.setTitle(site.name);   loadSiteDescriptors();}
  display();
  }


void SFTPv2Doc::onPickSite() {
  }


void SFTPv2Doc::onEditSite() {

  if (isLocked()) return;

  notePad.clear();

  if (site.edit())
            {site.logout();   site.login();   theApp.setTitle(site.name);   loadSiteDescriptors();}
  display();
  }


bool SFTPv2Doc::loadSiteDescriptors() {

String path;

  if (!localDirList.loadFromPC())
                     {notePad << site.lclRoot() << _T(" does not exist") << nCrlf; return false;}

  dataSource = BaseLineSrc;

  if (loadBaseLine() && !baseLineList.isEmpty()) notePad << _T("Base Line list loaded");
  else {
    baseLineList = localDirList;
    notePad << _T("*** Initializing base line list, should only happen once! ***");
    }

  notePad << nCrlf << nCrlf;

  baseLineList.display(_T("Base Line List"));   return true;
  }


void SFTPv2Doc::onCompSites() {

  if (isLocked()) return;

  updateList.clear();   site.getRmtDir();

  display();
  }


void SFTPv2Doc::comparePresences() {
UnitListIter lclIter(localDirList);
UnitListIter rmtIter(webDirList);
UnitDsc*     lclUnit;
UnitDsc*     rmtUnit;

  notePad.clear();   notePad << nClrTabs << nSetTab(65) << nSetTab(70);

  for (lclUnit = lclIter(), rmtUnit = rmtIter(); lclUnit; lclUnit = lclIter++) {
    if (rmtUnit && rmtUnit->key.dir) rmtUnit = rmtIter++;
    if (lclUnit->key.dir) continue;

    if (!rmtUnit) {toUpdate(lclUnit, PutOp, _T("== 0 Put: "));   notePad << nCrlf;   continue;}

    loop {

      if (rmtUnit->key.dir) {rmtUnit = rmtIter++;   continue;}

      notePad << lclUnit->key.path << _T(" ... ") << rmtUnit->key.path;

      if (*lclUnit == *rmtUnit) {rmtUnit = rmtIter++;   notePad << nCrlf;   break;}

      if (*lclUnit <  *rmtUnit)
                          {toUpdate(lclUnit, PutOp, _T(" < Put: "));   notePad << nCrlf;   break;}

      if (*lclUnit >  *rmtUnit)
            {toUpdate(rmtUnit, GetOp, _T(" > Get: "));   notePad << nCrlf;   rmtUnit = rmtIter++;}
      }
    }

    for ( ; rmtUnit; rmtUnit = rmtIter++) {
      notePad << _T("0 ... ") << rmtUnit->key.path;
      toUpdate(rmtUnit, GetOp, _T("0== Get: "));   notePad << nCrlf;
      }
  }


void SFTPv2Doc::onCompPrevious() {

  if (isLocked()) return;

  notePad.clear();   notePad << nClrTabs << nSetTab(65) << nSetTab(70);

  updateList.clear();   comparePrevious();   notePad << nCrlf;

  notePad << _T("Update List") << nCrlf << nCrlf;

  site.display(updateList);

  display();
  }


void SFTPv2Doc::comparePrevious() {
UnitListIter bslIter(baseLineList);
UnitListIter lclIter(localDirList);
UnitDsc*     bslUnit;
UnitDsc*     lclUnit;

  for (bslUnit = bslIter(), lclUnit = lclIter(); bslUnit; bslUnit = bslIter++) {

    if (!lclUnit) {toUpdate(bslUnit, DelOp, _T("== *0 Delete: "));   notePad << nCrlf;   continue;}

    loop {
      notePad << bslUnit->key.path << _T(" ... ") << lclUnit->key.path;

      if (*bslUnit == *lclUnit) {

        if (!bslUnit->key.dir &&
            (bslUnit->size != lclUnit->size || bslUnit->date < lclUnit->date))
                                                          toUpdate(lclUnit, PutOp, _T("== Put: "));
        notePad << nCrlf;   lclUnit = lclIter++;   break;
        }

      if (*bslUnit < *lclUnit) {
        if (bslUnit->key.dir) toUpdate(bslUnit, DelDirOp, _T("< Delete Dir: "));
        else                  toUpdate(bslUnit, DelOp,    _T("< Delete: "));

        notePad << nCrlf;  break;
        }

      if (*bslUnit > *lclUnit) {
        if (!lclUnit->key.dir) toUpdate(lclUnit, PutOp, _T("> Put: "));

        notePad << nCrlf;   lclUnit = lclIter++;
        }
      }
    }

    for (; lclUnit; lclUnit = lclIter++) {
      notePad << _T("0 ... ") << lclUnit->key.path;
      toUpdate(lclUnit, PutOp, _T("0== Put: "));   notePad << nCrlf;
      }
  }


void SFTPv2Doc::toUpdate(UnitDsc* ud, UnitOp op, TCchar* title) {
UnitDsc* updUnit = updateList.add(*ud);   updUnit->unitOp = op;
  notePad << nTab << title << updUnit->key.path;
  }


void SFTPv2Doc::onUpdate() {
UpdateDlg dlg(updateList);

  if (isLocked()) return;

  notePad.clear();

  if (dlg.DoModal() == IDOK && !updateList.isEmpty()) site.update();

  else {notePad << _T("No updates performed.") << nCrlf;   display();}
  }


void SFTPv2Doc::onViewDetails() {

  if (isLocked()) return;

  notePad.clear();

  notePad << _T("Update List") << nCrlf << nCrlf;
  site.display(updateList);   notePad << nCrlf;

  notePad << _T("Baseline List") << nCrlf << nCrlf;
  site.display(baseLineList);

  display();
  }


bool SFTPv2Doc::isLocked(bool prevent) {
  if (!workerThrd.isLocked()) return false;

  notePad << _T("Unable to perform command at this time") << nCrlf;   display();   return true;
  }


void SFTPv2Doc::defaultSite() {
String sect;

  if (!iniFile.read(GlobalSect, LastSiteKey, sect)) {display();  return;}

  if (!site.loadData(sect))                         {display();  return;}

  if (!site.login())                                {display();   return;}

  if (!loadSiteDescriptors())                       {display();   return;}

  display();
  }


void SFTPv2Doc::onEditCopy() {clipLine.load();}


void SFTPv2Doc::onSaveFile() {

  if (isLocked()) return;

//  if (!saveBaseLine()) notePad << _T("CSV file not saved: ") << nCrlf;

  display();
  }


void SFTPv2Doc::onSaveNotePad() {

  if (isLocked()) return;

  dataSource = NotePadSrc;   if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);

  display(StoreSrc);
  }


void SFTPv2Doc::display(DataSource ds) {dataSource = ds; invalidate();}


void SFTPv2Doc::loadCNG(CNGblock& cng) {
String path = pswdPath();

  cngBlk = &cng;   dataSource = NamePswdSrc;   if (OnOpenDocument(path)) return;

  pathDlgDsc(_T("Site Password Data"), path, _T("cng"), _T("*.cng"));

  if (!setOpenPath(pathDlgDsc)) return;

  if (!OnOpenDocument(path)) messageBox(_T(" No Password File!"));
  }


void SFTPv2Doc::saveCNG(CNGblock* cng) {
String path = pswdPath();

  cngBlk = cng;    dataSource = NamePswdSrc;   if (OnSaveDocument(path)) return;

  pathDlgDsc(_T("Save Password Data"), path, _T("cng"), _T("*.cng"));

  if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);
  }


void SFTPv2Doc::saveFile(TCchar* title, TCchar* suffix, TCchar* fileType) {
String fileName = path;
int    pos      = fileName.find_last_of(_T('\\'));
String ext      = _T("*."); ext += fileType;
String ttl      = title;    ttl += _T(" Output");

  fileName = fileName.substr(pos+1);   pos = fileName.find_first_of(_T('.'));
  fileName = fileName.substr(0, pos);  fileName += suffix;

  pathDlgDsc(ttl, fileName, fileType, ext);

  if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);
  }


bool SFTPv2Doc::loadBaseLine() {
String s = theApp.roamingPath() + removeSpaces(site.name) + _T(".csv")  ;

  dataSource = BaseLineSrc;   return OnOpenDocument(s);
  }


bool SFTPv2Doc::saveBaseLine() {
String s = theApp.roamingPath() + removeSpaces(site.name) + _T(".csv")  ;

  dataSource = BaseLineSrc;   return OnSaveDocument(s);
  }


// UglyDoc serialization

void SFTPv2Doc::serialize(Archive& ar) {

  if (ar.isStoring())
    switch(dataSource) {
      case NotePadSrc : notePad.archive(ar);   return;
      case NamePswdSrc: cngBlk->store(ar);     return;
      case WebSrc     : sftpSSL.store(ar);     return;          // Store buffer in local file
      case BaseLineSrc: baseLineList.saveCSV(ar);
      default         : return;
      }

  else
    switch(dataSource) {
      case NamePswdSrc: cngBlk->load(ar);               return;
      case BaseLineSrc: baseLineList.loadFromCSV(ar);   return;
      case WebSrc     : sftpSSL.load(ar);               return; // Load buffer from local file
      default         : return;
      }
  }


// SFTPv2Doc diagnostics

#ifdef _DEBUG
void SFTPv2Doc::AssertValid() const {         CDocument::AssertValid();}
void SFTPv2Doc::Dump(CDumpContext& dc) const {CDocument::Dump(dc);}
#endif //_DEBUG




///-----------------
                 #if 0
#ifdef Examples
#include "Store.h"
#endif
#ifdef Examples
  ON_COMMAND(      ID_Button,      &myButton)
  ON_CBN_KILLFOCUS(ID_EditBox,     &OnTBEditBox)
  ON_COMMAND(      ID_EditBox,     &OnTBEditBox)

  ON_COMMAND(      ID_Menu,        &onOption11)
  ON_COMMAND(      ID_Option11,    &onOption11)
  ON_COMMAND(      ID_Option12,    &onOption12)
  ON_COMMAND(      ID_Option13,    &onOption13)

  ON_COMMAND(      ID_Menu1,       &onOption21)
  ON_COMMAND(      ID_Option21,    &onOption21)
  ON_COMMAND(      ID_Option22,    &onOption22)
  ON_COMMAND(      ID_Option23,    &onOption23)

  ON_CBN_SELCHANGE(ID_CboBx,       &OnComboBoxChng)
  ON_COMMAND(      ID_CboBx,       &OnComboBoxChng)
#endif
#ifdef Examples
  ON_COMMAND(      ID_Test,        &OnTest)
  ON_COMMAND(      ID_SelDataStr,  &displayDataStore)
#endif
#ifdef Examples

static CbxItem cbxText[] = {{_T("Zeta"),     1},
                            {_T("Beta"),     2},
                            {_T("Alpha"),    3},
                            {_T("Omega"),    4},
                            {_T("Phi"),      5},
                            {_T("Mu"),       6},
                            {_T("Xi"),       7},
                            {_T("Omicron"),  8},
                            {_T("Pi"),       9},
                            {_T("Rho"),     10},
                            {_T("Sigma"),   11},
                            {_T("Nu"),      12},
                            {_T("Kappa"),   13},
                            {_T("Iota"),    14},
                            {_T("This is a reallyt long Greek"), 15}
                            };
static TCchar* CbxCaption = _T("Greeks");


void SFTPv2Doc::myButton() {
MyToolBar& toolBar = getToolBar();
int        i;
int        n;

  for (i = 0, n = noElements(cbxText); i < n; i++) {
    CbxItem& item = cbxText[i];

    toolBar.addCbxItemSorted(ID_CboBx, item.txt, item.data);
    }
  toolBar.setCaption(ID_CboBx, CbxCaption);
  toolBar.setWidth(ID_CboBx);

  notePad << _T("Loaded ") << CbxCaption << _T(" into ComboBx") << nCrlf;  display();
  }


void SFTPv2Doc::OnComboBoxChng() {
MyToolBar& toolBar = getToolBar();
String     s;
int        x;

  if (toolBar.getCurSel(ID_CboBx, s, x))
                         notePad << _T("On Change, Item = ") << s << _T(", Data = ") << x << nCrlf;
  display();
  }



void SFTPv2Doc::OnTBEditBox() {
MyToolBar& toolBar = getToolBar();
CString    cs      = toolBar.getText(ID_EditBox);
String     s       = cs;

  if (!s.isEmpty()) notePad << s << nCrlf;

  display();
  }


void SFTPv2Doc::myButton1() {
MyToolBar& toolBar = getToolBar();
CString    cs      = toolBar.getText(ID_EditBox);
String     s       = cs;

  if (!s.isEmpty()) notePad << s << nCrlf;

  display();
  }


void SFTPv2Doc::onOption11() {notePad << _T("Option 11") << nCrlf; display();}
void SFTPv2Doc::onOption12() {notePad << _T("Option 12") << nCrlf; display();}
void SFTPv2Doc::onOption13() {notePad << _T("Option 13") << nCrlf; wholePage(); display();}


void SFTPv2Doc::onOption21() {notePad << _T("Option 21") << nCrlf; display();}
void SFTPv2Doc::onOption22() {notePad << _T("Option 22") << nCrlf; display();}
void SFTPv2Doc::onOption23() {notePad << _T("Option 23") << nCrlf; display();}


void SFTPv2Doc::OnTestEditBoxes() {display();}


// SFTPv2Doc commands



void SFTPv2Doc::wholePage() {
int    i;
int    j;
String s;

  notePad << nSetRTab(4) << nSetTab(6) << nSetTab(8) << nSetRTab(12) << nSetTab(14);

  for (i = 0; i < 100; i++) {
    s.format(_T("%3i:"), i);

    notePad << nTab << s << nTab << _T("X") << nTab << _T("Y") << nTab << i << nTab;

    for (j = i+1; j < i+68; j++)
                              {s.format(_T("%i"), j);   s = s.substr(s.length()-1);  notePad << s;}
    notePad << nCrlf;
    }
  }


void SFTPv2Doc::displayDataStore() {display(StoreSrc);}

#endif
#ifdef Examples
  store.setName(pathDlgDsc.name);
#endif
#ifdef Examples
      case StoreSrc   : store.store(ar); return;
      case StrRptSrc  : view()->storeRpt().txtOut( ar, 1.35); return;
#endif
#ifdef Examples
      case StoreSrc : store.load(ar); return;
#endif
#if 0
void SFTPv2Doc::onFileOpen() {

  notePad.clear();   dataSource = StoreSrc;

  pathDlgDsc(_T("Ugly Example"), pathDlgDsc.name, _T("txt"), _T("*.txt"));

  if (!setOpenPath(pathDlgDsc)) return;

  pathDlgDsc.name = getMainName(path);

  if (!OnOpenDocument(path)) messageBox(_T(" Not Loaded!"));


  display(StoreSrc);
  }
#endif
#if 0
  if (!site.getLocalPath()) return;

  iniFile.readString(GlobalSect, LocalWebPath, path);

  getDirPathDlg(_T("Web Site"), path);   fixLocalPath(path);

  String pth = ::getPath(path);

  iniFile.writeString(GlobalSect, LocalWebPath, pth);   siteID.localRoot = pth;
#endif
#if 0
void SFTPv2Doc::onSetNamePswd() {
NamePswdDlg dlg;
String      np;
String      path;
CNG         cng;

  while (dlg.DoModal() == IDOK) {
    notePad << _T("Name: ") << dlg.name << _T(", Password: ") << dlg.pswd << nCrlf;

    if (dlg.pswd.find(_T(';')) >= 0 || dlg.name.find(_T(';') >= 0) >= 0) continue;

    np = dlg.name + _T(";") + dlg.pswd;

    cngBlk = cng(np, MyPassword);

    expunge(np);
    expunge(dlg.name);
    expunge(dlg.pswd);

    path = theApp.roamingPath() + _T("NamePassword.cng");

    dataSource = NamePswdSrc;   pathDlgDsc(_T("Save Password"), path, _T("cng"), _T("*.cng"));

    if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);

    cngBlk->expunge();   break;
    }

  display();
  }
#endif
#if 0
PickTransaction pt;  pt.test();
#else
#endif
#endif
#if 1
#else
      updUnit = updateList.add(*lclUnit);   updUnit->unitOp = PutOp;

      notePad << nTab << updUnit->key.path << nCrlf;
#endif
#if 1
#else
        updUnit = updateList.add(*lclUnit);   updUnit->unitOp = PutOp;
        notePad << nTab << _T(" < Update: ") << updUnit->key.path << nCrlf;
#endif
#if 1
#else
        updUnit = updateList.add(*rmtUnit);   updUnit->unitOp = GetOp;
        notePad << nTab << _T(" > Update: ") << updUnit->key.path << nCrlf;
#endif
#if 1
#else
      updUnit = updateList.add(*rmtUnit);   updUnit->unitOp = GetOp;
      notePad << nTab << _T(" > Update: ") << updUnit->key.path << nCrlf;
#endif
#if 1
#else
      updUnit = updateList.add(*bslUnit);   updUnit->unitOp = DelOp;
      notePad << updUnit->key.path << nTab << _T("== *0 ") << nCrlf;
#endif
#if 1
#else
          updUnit = updateList.add(*bslUnit);   updUnit->unitOp = PutOp;
          notePad << nTab << _T("== Update: ");
#endif
#if 1
#else
          updUnit = updateList.add(*bslUnit);   updUnit->unitOp = DelDirOp;
          notePad << nTab << _T("< Delete Dir: ");
#endif
#if 1
#else
          updUnit = updateList.add(*bslUnit);   updUnit->unitOp = DelOp;
          notePad << nTab << _T("< Delete: ");
#endif
#if 1
#else
          updUnit = updateList.add(*lclUnit);   updUnit->unitOp = PutOp;
          notePad << nTab << _T("> Update: ") << updUnit->key.path;
#endif

