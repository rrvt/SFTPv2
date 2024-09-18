
// SFTPv2View.cpp : implementation of the SFTPv2View class


#include "pch.h"
#include "SFTPv2View.h"
#include "SFTPv2.h"
#include "SFTPv2Doc.h"
#include "ClipLine.h"
#include "IniFile.h"
#include "OptionsDlg.h"
#include "Resource.h"
#include "Resources.h"
#include "SendMsg.h"
#include "RptOrientDlgTwo.h"


static TCchar* StrOrietnKey = _T("Store");


IMPLEMENT_DYNCREATE(SFTPv2View, CScrView)

BEGIN_MESSAGE_MAP(SFTPv2View, CScrView)
  ON_COMMAND(ID_Options,     &onOptions)
  ON_COMMAND(ID_Orientation, &onRptOrietn)

  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()

  ON_WM_CONTEXTMENU()                     // Right Mouse Popup Menu
  ON_COMMAND(ID_CopySel, &onCopy)
  ON_COMMAND(ID_Pup1,    &onPup1)
  ON_COMMAND(ID_Pup2,    &onPup2)

END_MESSAGE_MAP()


SFTPv2View::SFTPv2View() noexcept : scrollWdw(false) {
ResourceData res;
String       pn;
  if (res.getProductName(pn)) prtNote.setTitle(pn);

  sub.LoadMenu(IDR_RtMouseMenu);
  menu.CreatePopupMenu();
  menu.AppendMenu(MF_POPUP, (UINT_PTR) sub.GetSafeHmenu(), _T(""));        //

  sub.Detach();
  }


BOOL SFTPv2View::PreCreateWindow(CREATESTRUCT& cs) {return CScrView::PreCreateWindow(cs);}


void SFTPv2View::OnInitialUpdate() {CScrView::OnInitialUpdate();}


void SFTPv2View::onOptions() {
OptionsDlg dlg;

  if (printer.name.isEmpty()) printer.load(0);

  if (dlg.DoModal() == IDOK) pMgr.setFontScale(printer.scale);
  }


void SFTPv2View::onRptOrietn() {
RptOrietnDlg dlg;

  dlg.lbl01 = _T("Store:");
  dlg.ntpd = printer.toStg(prtNote.prtrOrietn);

  if (dlg.DoModal() == IDOK) {
    prtNote.prtrOrietn  = printer.toOrient(dlg.ntpd);
    saveRptOrietn();
    }
  }


void SFTPv2View::initRptOrietn()
  { }


void SFTPv2View::saveRptOrietn() {
  saveNoteOrietn();

  }


void SFTPv2View::onDisplayOutput() {

  switch(doc()->dataSrc()) {
    case NotePadSrc : dspNote.display(*this); break;
    }
  }


void SFTPv2View::displayHeader(DevBase& dev) {
  switch(doc()->dataSrc()) {
    case NotePadSrc   : dspNote.dspHeader(dev);   break;
    }
  }


void SFTPv2View::displayFooter(DevBase& dev) {
  switch(doc()->dataSrc()) {
    case NotePadSrc   : dspNote.dspFooter(dev);   break;
    }

  if (scrollWdw) sendMsg(WM_VSCROLL, SB_BOTTOM, 0);         /// ** Curious
  }



void SFTPv2View::onPreparePrinting(CPrintInfo* info) {

  switch(doc()->dataSrc()) {
    case NotePadSrc : prtNote.onPreparePrinting(info);     break;
    }
  }


// Perpare output (i.e. report) then start the output with the call to SCrView

void SFTPv2View::onBeginPrinting() {

  switch(doc()->dataSrc()) {
    case NotePadSrc : prtNote.onBeginPrinting(*this);  break;
    }
  }


void SFTPv2View::printHeader(DevBase& dev, int pageNo) {
  switch(doc()->dataSrc()) {
    case NotePadSrc: prtNote.prtHeader(dev, pageNo);   break;
    }
  }


// The footer is injected into the printed output, so the output goes directly to the device.
// The output streaming functions are very similar to NotePad's streaming functions so it should
// not be a great hardship to construct a footer.

void SFTPv2View::printFooter(DevBase& dev, int pageNo) {
  switch(doc()->dataSrc()) {
    case NotePadSrc : prtNote.prtFooter(dev, pageNo);  break;
    }
  }



void SFTPv2View::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {

  switch(doc()->dataSrc()) {
    case NotePadSrc : break;
    case StoreSrc   : break;
    }
  }


void SFTPv2View::OnSetFocus(CWnd* pOldWnd) {

  CScrView::OnSetFocus(pOldWnd);

  switch(doc()->dataSrc()) {
    case NotePadSrc : break;
    case StoreSrc   : break;
    }
  }


void SFTPv2View::OnLButtonDown(UINT nFlags, CPoint point)
                   {clipLine.set(point);   invalidate();   CScrView::OnLButtonDown(nFlags, point);}


void SFTPv2View::OnLButtonDblClk(UINT nFlags, CPoint point) {
  clipLine.set(point);   RedrawWindow();   clipLine.load();

  CScrView::OnLButtonDblClk(nFlags, point);
  }


void SFTPv2View::OnContextMenu(CWnd* /*pWnd*/, CPoint point) {
CRect  rect;
CMenu* popup;
CWnd*  pWndPopupOwner = this;

  if (point.x == -1 && point.y == -1)
        {GetClientRect(rect);  ClientToScreen(rect);  point = rect.TopLeft();  point.Offset(5, 5);}

  popup = menu.GetSubMenu(0);   if (!popup) return;

  while (pWndPopupOwner->GetStyle() & WS_CHILD) pWndPopupOwner = pWndPopupOwner->GetParent();

  popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
  }


void SFTPv2View::onCopy() {clipLine.load();  invalidate();}


void SFTPv2View::onPup1() {  }


void SFTPv2View::onPup2() {  }


// SFTPv2View diagnostics

#ifdef _DEBUG
void SFTPv2View::AssertValid() const          {CScrollView::AssertValid();}
void SFTPv2View::Dump(CDumpContext& dc) const {CScrollView::Dump(dc);}
                                                                    // non-debug version is inline
SFTPv2Doc* SFTPv2View::GetDocument() const {              // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SFTPv2Doc)));
  return (SFTPv2Doc*)m_pDocument;
  }
#endif //_DEBUG






///--------------------------

#ifdef Examples
SFTPv2View::SFTPv2View() noexcept : dspStore(dMgr.getNotePad()),
                                              prtStore(pMgr.getNotePad()) {
#else
#endif
#ifdef Examples
  dlg.rpt1 = printer.toStg(prtStore.prtrOrietn);
#endif
#ifdef Examples
    prtStore.prtrOrietn = printer.toOrient(dlg.rpt1);
#endif
#ifdef Examples
  {prtStore.prtrOrietn = (PrtrOrient) iniFile.readInt(RptOrietnSect, StrOrietnKey, PortOrient);}
#else
#endif
#ifdef Examples
  iniFile.write(RptOrietnSect, StrOrietnKey,  (int) prtStore.prtrOrietn);
#endif
#ifdef Examples
    case StoreSrc   : dspStore.display(*this); break;
#endif
#ifdef Examples
    case StoreSrc     : dspStore.dspHeader(dev);  break;
#endif
#ifdef Examples
    case StoreSrc     : dspStore.dspFooter(dev);  break;
#endif
#ifdef Examples
    case StoreSrc   : prtStore.onPreparePrinting(info);    break;
#endif
#ifdef Examples
    case StoreSrc   : prtStore.onBeginPrinting(*this); break;
#endif
#ifdef Examples
    case StoreSrc  : dspStore.prtHeader(dev, pageNo);  break;
#endif
#ifdef Examples
    case StoreSrc   : prtStore.prtFooter(dev, pageNo); break;
#endif

