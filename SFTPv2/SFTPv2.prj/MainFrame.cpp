// MainFrm.cpp : implementation of the MainFrame class


#include "pch.h"
#include "MainFrame.h"
#include "AboutDlg.h"
#include "SFTPv2.h"
#include "SFTPv2Doc.h"
#include "SFTPv2View.h"


// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWndEx)

  ON_WM_CREATE()
  ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &OnResetToolBar)
  ON_WM_SYSCOMMAND()

  ON_WM_MOVE()
  ON_WM_SIZE()


  ON_MESSAGE(ID_CompSitesMsg,  &onCompSites)
  ON_MESSAGE(ID_DspRmtSiteMsg, &onDspRmtSite)
  ON_MESSAGE(ID_UpdateMsg,     &onUpdateMsg)
  ON_MESSAGE(ID_StepPrgBarMsg, &onStepPrgBarMsg)
  ON_MESSAGE(ID_SetWdwScroll,  &onSetWdwScroll)
  ON_MESSAGE(ID_DisplayMsg,    &onDisplayMsg)

END_MESSAGE_MAP()


static UINT indicators[] = {
  ID_SEPARATOR,                                                 // status line indicator
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
  };


MainFrame::MainFrame() noexcept : isInitialized(false) { }

MainFrame::~MainFrame() {winPos.~WinPos();}


BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs) {

  cs.style &= ~FWS_ADDTOTITLE;  cs.lpszName = _T("Secure File Transfer Protocol v2");
                                                               // Sets the default title left part
  return CFrameWndEx::PreCreateWindow(cs);
  }


int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
CRect winRect;

  if (CFrameWndEx::OnCreate(lpCreateStruct) == -1) return -1;

  if (!menuBar.Create(this)) {TRACE0("Failed to create menubar\n"); return -1;}

  CMFCPopupMenu::SetForceMenuFocus(FALSE);

  if (!toolBar.create(this, IDR_MAINFRAME)) {TRACE0("Failed to create toolbar\n"); return -1;}

  addAboutToSysMenu(IDD_AboutBox, IDS_AboutBox);

  if (!statusBar.Create(this)) {TRACE0("Failed to create status bar\n"); return -1;}

  statusBar.SetIndicators(indicators, noElements(indicators));

  GetWindowRect(&winRect);   winPos.initialPos(this, winRect);

  DockPane(&menuBar);   DockPane(&toolBar);

  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
                                                                  // Affects look of toolbar, etc.
  isInitialized = true;   return 0;
  }


void MainFrame::OnSysCommand(UINT nID, LPARAM lParam) {

  if ((nID & 0xFFF0) == sysAboutID) {AboutDlg aboutDlg; aboutDlg.DoModal(); return;}

  CMainFrm::OnSysCommand(nID, lParam);
  }


void MainFrame::OnMove(int x, int y)
    {CRect winRect;   GetWindowRect(&winRect);   winPos.set(winRect);   CFrameWndEx::OnMove(x, y);}


void MainFrame::OnSize(UINT nType, int cx, int cy) {
CRect r;

  GetWindowRect(&r);

  if (!isInitialized) {winPos.setInvBdrs(r, cx, cy);   return;}

  winPos.set(cx, cy);   CFrameWndEx::OnSize(nType, cx, cy);
  }


// MainFrame message handlers

LRESULT MainFrame::OnResetToolBar(WPARAM wParam, LPARAM lParam) {setupToolBar();  return 0;}


void MainFrame::setupToolBar() {
CRect winRect;   GetWindowRect(&winRect);   toolBar.set(winRect);

  toolBar.addMenu(ID_DisplayList, IDR_DisplayList, 6);
  }


// Progress Bar functions


void MainFrame::startPrgBar(uint noSteps) {
CRect rect;
CRect winRect;

  GetClientRect(winRect);
  rect.left  = 20;                   rect.top = winRect.bottom / 2 - 5;
  rect.right = winRect.right - 20;   rect.bottom = rect.top + 10;

  progressBar.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rect, this, IDC_ProgCtrl);

  if      (0 <= noSteps && noSteps <  4) noSteps = 25;
  else if (4 <= noSteps && noSteps < 10) noSteps = 50;
  else                                   noSteps += noSteps / 10;

  progressBar.SetRange(0, noSteps);   progressBar.SetPos(0);   progressBar.SetStep(1);
  }


LRESULT MainFrame::onStepPrgBarMsg(WPARAM wParam, LPARAM lParam) {progressBar.StepIt();  return 0;}


LRESULT MainFrame::onSetWdwScroll(WPARAM wParam, LPARAM lParam)
                                                         {view()->setWidwScroll(wParam); return 0;}


LRESULT MainFrame::onDisplayMsg(WPARAM wParam, LPARAM lParam) {doc()->display();   return 0;}



// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const          {CFrameWndEx::AssertValid();}
void MainFrame::Dump(CDumpContext& dc) const {CFrameWndEx::Dump(dc);}
#endif //_DEBUG


