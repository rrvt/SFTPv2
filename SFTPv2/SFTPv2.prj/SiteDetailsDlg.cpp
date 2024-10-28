// Web Login Dialog Box


#include "pch.h"
#include "SiteDetailsDlg.h"
#include "AppUtilities.h"
#include "MessageBox.h"
#include "Resource.h"
#include "GetPathDlg.h"
#include "SFTPv2.h"
#include "SFTPv2Doc.h"
#include "Site.h"
#include "Utilities.h"




IMPLEMENT_DYNAMIC(SiteDetailsDlg, CDialogEx)


BEGIN_MESSAGE_MAP(SiteDetailsDlg, CDialogEx)
    ON_BN_CLICKED(  IDC_LclRootBtn, &OnLclRootBtn)
    ON_BN_CLICKED(  IDC_LoadIdPswd, &OnLoadIdPswd)
    ON_EN_KILLFOCUS(IDC_Name,       &OnLeaveName)
END_MESSAGE_MAP()


SiteDetailsDlg::SiteDetailsDlg(CWnd* pParent) : CDialogEx(IDD_SiteDetails, pParent),
                          urlName(_T("")), userID(_T("")), password(_T("")), remoteRoot(_T("")) { }

SiteDetailsDlg::~SiteDetailsDlg() { }


BOOL SiteDetailsDlg::OnInitDialog()
  {getIdPswd();   CDialogEx::OnInitDialog();   localRoot.SetWindowText(rootTxt);   return TRUE;}


void SiteDetailsDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(   pDX, IDC_Name,       name);
  DDX_Control(pDX, IDC_LclRoot,    localRoot);
  DDX_Control(pDX, IDC_LclRootBtn, localRootBtn);
  DDX_Text(   pDX, IDC_URLName,    urlName);
  DDX_Text(   pDX, IDC_UserID,     userID);
  DDX_Text(   pDX, IDC_Password,   password);
  DDX_Text(   pDX, IDC_RemoteRoot, remoteRoot);
  DDX_Control(pDX, IDC_Name,       nameCtl);
  DDX_Control(pDX, IDC_UserID,     userIDctl);
  DDX_Control(pDX, IDC_Password,   passwordCtl);
  }


void SiteDetailsDlg::OnLeaveName() {
String id;
String pswd;

  nameCtl.GetWindowText(name);   if (name.isEmpty()) return;

  if (name.find(_T(';')) >= 0)
              {messageBox(_T("Name Cannot contain a semicolon"));   nameCtl.SetFocus();   return;}

  name = fixName(name);   nameCtl.SetWindowText(name);

#if 1

  if (getIdPswd()) {userIDctl.SetWindowText(userID);   passwordCtl.SetWindowText(password);}

#else
  if (doc()->getNamePassword(name, id, pswd)) {
    userID = id;   password = pswd;
    userIDctl.SetWindowText(userID);   passwordCtl.SetWindowText(password);
    }
#endif
  }


bool SiteDetailsDlg::getIdPswd() {
String id;
String pswd;

  if (!doc()->getNamePassword(name, id, pswd)) return false;

  userID = id;   password = pswd;  expunge(id);   expunge(pswd);   return true;
  }


String SiteDetailsDlg::fixName(TCchar* name) {
String s = name;   s.lowerCase();
String t = name;
int    pos;

  pos = s.find(_T("site"));   if (pos >= 0) t = t.substr(0, pos);
  pos = s.find(_T("web"));    if (pos >= 0) t = t.substr(0, pos);

  t.trim();   t += _T(" Web Site");   t.trim();   return t;
  }


void SiteDetailsDlg::OnLclRootBtn() {
String path;

  getDirPathDlg(_T("Local Root Path"), path);   //fixLocalPath(path);

  rootTxt = path;  localRoot.SetWindowText(rootTxt);
  }


void SiteDetailsDlg::OnLoadIdPswd() {
String id;
String pswd;

  nameCtl.GetWindowText(name);

  if (doc()->loadNamePassword(name, id, pswd)) {
    userID = id;   password = pswd;
    userIDctl.SetWindowText(userID);   passwordCtl.SetWindowText(password);
    }
  }


void SiteDetailsDlg::OnOK() {

  localRoot.GetWindowText(rootTxt);

  CDialogEx::OnOK();
  }
