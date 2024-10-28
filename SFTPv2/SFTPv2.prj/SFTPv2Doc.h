// SFTPv2Doc.h : interface of the SFTPv2Doc class


#pragma once
#include "CDoc.h"
#include "CNG.h"
#include "MainFrame.h"
#include "SFTPv2.h"
#include "Site.h"
#include "UnitDsc.h"


enum DataSource {NotePadSrc, NamePswdSrc, BaseLineSrc, WebSrc, StoreSrc};


class SFTPv2Doc : public CDoc {

PathDlgDsc  pathDlgDsc;
String      fileName;

DataSource  dataSource;

CNGblock    cngBlock;
CNGblock*   cngBlk;
bool        cngFileFound;

protected: // create from serialization only

  SFTPv2Doc() noexcept;
  DECLARE_DYNCREATE(SFTPv2Doc)

public:

  virtual   ~SFTPv2Doc();

  bool       isLocked(bool prevent = false);

  void       defaultSite();

  DataSource dataSrc() {return dataSource;}
  void       display(DataSource ds = NotePadSrc);

  void       comparePresences();

  bool       loadBaseLine();
  bool       saveBaseLine();

  bool       loadNamePassword(TCchar* siteName, String& name, String& pswd);
  bool       getNamePassword( TCchar* siteName, String& name, String& pswd);
  bool       getCNG( TCchar* siteName);
  bool       openCNG(TCchar* siteName);
  void       saveNamePassword(TCchar* siteName, TCchar* name, TCchar* pswd);

  bool       loadXfrBuffer(TCchar* path)  {dataSource = WebSrc; return OnOpenDocument(path);}
  bool       storeXfrBuffer(TCchar* path) {dataSource = WebSrc; return OnSaveDocument(path);}

  void       dspBaseLineList();
  void       dspRmtSite();

  virtual void serialize(Archive& ar);

// Implementation

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

private:

  void       clearLists();

  void       comparePrevious();
  void       toUpdate(UnitDsc* ud, UnitOp op, TCchar* title);

  bool       loadSiteLists();
  String     pswdPath(    TCchar* siteName) {return mkAppPath(siteName, _T(".cng"));}
  String     baseLinePath(TCchar* siteName) {return mkAppPath(siteName, _T(".csv"));}
  String     mkAppPath(TCchar* name, TCchar* suffix);

  bool       parseCng(String& name, String& pswd);

  void       saveCNG(TCchar* siteName);
  void       saveFile(TCchar* title, TCchar* suffix, TCchar* fileType);

// Generated message map functions

protected:

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void onNewSite();
  afx_msg void onPickSite();
  afx_msg void onEditSite();
  afx_msg void onDeleteSite();

  afx_msg void onCompSites();
  afx_msg void onCompPrevious();
  afx_msg void onUpdate();

  afx_msg void onDspBaseList();
  afx_msg void onDspLclList();
  afx_msg void onDspRmtList();

  afx_msg void onSaveNotePad();

  afx_msg void onEditCopy();
  };




/// ----------------------------

#ifdef Examples
  afx_msg void displayDataStore();
  afx_msg void myButton();

  afx_msg void OnComboBoxChng();

  afx_msg void OnTBEditBox();
  afx_msg void myButton1();
  afx_msg void onOption11();
  afx_msg void onOption12();
  afx_msg void onOption13();
  afx_msg void onOption21();
  afx_msg void onOption22();
  afx_msg void onOption23();
  afx_msg void OnTestEditBoxes();
#endif

