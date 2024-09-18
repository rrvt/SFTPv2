// SFTPv2Doc.h : interface of the SFTPv2Doc class


#pragma once
#include "CDoc.h"
#include "CNG.h"
#include "MainFrame.h"
#include "SFTPv2.h"
#include "Site.h"


enum DataSource {NotePadSrc, NamePswdSrc, BaseLineSrc, WebSrc, StoreSrc};


class SFTPv2Doc : public CDoc {

PathDlgDsc  pathDlgDsc;

DataSource  dataSource;

CNGblock*   cngBlk;

protected: // create from serialization only

  SFTPv2Doc() noexcept;
  DECLARE_DYNCREATE(SFTPv2Doc)

public:

  virtual   ~SFTPv2Doc();

  bool       isLocked(bool prevent = false);

  void       defaultSite();

  DataSource dataSrc() {return dataSource;}
  void       display(DataSource ds = NotePadSrc);

  void       loadCNG(CNGblock& cng);
  void       saveCNG(CNGblock* cng);

  bool       loadXfrBuffer(TCchar* path)  {dataSource = WebSrc; return OnOpenDocument(path);}
  bool       storeXfrBuffer(TCchar* path) {dataSource = WebSrc; return OnSaveDocument(path);}

  virtual void serialize(Archive& ar);

// Implementation
public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

private:

  bool   loadSiteDescriptors();
  String pswdPath() {return theApp.roamingPath() + site.dataFileName() + _T("Data.cng");}

  void   saveFile(TCchar* title, TCchar* suffix, TCchar* fileType);

// Generated message map functions

protected:

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void onNewSite();
  afx_msg void onPickSite();
  afx_msg void onEditSite();
  afx_msg void onUpdate();

  afx_msg void onViewDetails();

  afx_msg void onSaveFile();
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

