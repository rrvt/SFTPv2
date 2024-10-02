// Site Update Dialog


#include "pch.h"
#include "UpdateDlg.h"
#include "UnitList.h"
#include "Utilities.h"


// UpdateDlg dialog

IMPLEMENT_DYNAMIC(UpdateDlg, CDialogEx)


UpdateDlg::UpdateDlg(UnitList& ul, CWnd* pParent) :
                                              CDialogEx(IDD_SiteUpdate, pParent), unitList(ul) { }

UpdateDlg::~UpdateDlg() { }


BOOL UpdateDlg::OnInitDialog() {
UnitListIter iter(unitList);
UnitDsc*     ud;
int          cnt;
String       s;
int          index;                  //PutOp,   GetOp,   MkLclDirOp,   MkRmtDirOp,   DelOp,   DelDirOp
int          ht;
CRect        rect;

  CDialogEx::OnInitDialog();

  for (ud = iter(), cnt = 0; ud; ud = iter++) {
    switch (ud->unitOp) {
      case PutOp    : s = _T(" Put File To Web:   ");   break;

      case GetOp    : s = _T(" Get File From Web: ");   break;

      case DelOp    : s = _T(" Delete Web File:   ");   break;


      case DelDirOp : s = _T(" Delete Web Dir:    ");   break;
      default       : continue;
      }

    s += ud->key.path;

    index = listCtrl.AddString(s);   listCtrl.SetItemData(index, (DWORD_PTR) ud);    cnt++;
    }

  // Curious -- GetClientRect produces a vertical height that does not work well with the scroll
  // bar This code is used to produce a scroll bar and to adjust the scroll thumb and page size

  ht = listCtrl.GetItemHeight(0);   listCtrl.GetClientRect(rect);

  if (cnt * ht >= rect.Height() - 3*ht) {
    SCROLLINFO sbInfo = {sizeof(SCROLLINFO), SIF_ALL};

    listCtrl.ShowScrollBar(SB_VERT, TRUE);
    listCtrl.GetScrollInfo(SB_VERT, &sbInfo);   sbInfo.nPage -= 3;
    listCtrl.SetScrollInfo(SB_VERT, &sbInfo);
    }

  return TRUE;
  }


void UpdateDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_UpdateList, listCtrl);
  }


BEGIN_MESSAGE_MAP(UpdateDlg, CDialogEx)
  ON_BN_CLICKED(IDC_UpdateAll, &UpdateDlg::OnUpdateAll)
END_MESSAGE_MAP()


// UpdateDlg message handlers


void UpdateDlg::OnOK() {
int       n = listCtrl.GetCount();
int       i;
UnitDsc* ud;

  for (i = 0; i < n; i++) {

    ud = (UnitDsc*) listCtrl.GetItemData(i);

    if (listCtrl.GetCheck(i) != BST_CHECKED) unitList.delRcd(*ud);
    }

  CDialogEx::OnOK();
  }


void UpdateDlg::OnUpdateAll() {CDialogEx::OnOK();}
