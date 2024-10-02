// Site Update Dialog -- Presents the files that the user may consider for updating


#pragma once
#include "Resource.h"

class UnitList;


// UpdateDlg dialog

class UpdateDlg : public CDialogEx {

  DECLARE_DYNAMIC(UpdateDlg)

UnitList& unitList;

public:
CCheckListBox listCtrl;

               UpdateDlg(UnitList& ul, CWnd* pParent = nullptr);
  virtual     ~UpdateDlg();

  virtual BOOL OnInitDialog();
  virtual void OnOK();

  afx_msg void OnUpdateAll();

// Dialog Data

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_SiteUpdate };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
  };
