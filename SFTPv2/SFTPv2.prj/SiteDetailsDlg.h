// Site Login Dialog Box -- Allows the user to supply the information needed to login to the web
// host

#pragma once


class SiteDetailsDlg : public CDialogEx {

  DECLARE_DYNAMIC(SiteDetailsDlg)

public:
Cstring name;
Cstring rootTxt;
Cstring urlName;
Cstring userID;
Cstring password;
Cstring remoteRoot;
CButton localRootBtn;
CStatic localRoot;
CEdit   nameCtl;
CEdit   userIDctl;
CEdit   passwordCtl;

           SiteDetailsDlg(CWnd* pParent = nullptr);              // standard constructor
  virtual ~SiteDetailsDlg();

  virtual BOOL OnInitDialog();
  virtual void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_SiteDetails };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);            // DDX/DDV support

  DECLARE_MESSAGE_MAP()

private:

  bool   getIdPswd();
  String fixName(TCchar* name);

public:

  afx_msg void OnLclRootBtn();
  afx_msg void OnLoadIdPswd();
  afx_msg void OnLeaveName();
  };
