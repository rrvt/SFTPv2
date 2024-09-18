// SFTPv2View.h : interface of the SFTPv2View class


#pragma once
#include "CScrView.h"
#include "NotePadRpt.h"


class SFTPv2Doc;


class SFTPv2View : public CScrView {

CMenu      menu;
CMenu      sub;
bool       scrollWdw;

protected: // create from serialization only

  SFTPv2View() noexcept;

  DECLARE_DYNCREATE(SFTPv2View)

public:

  virtual        ~SFTPv2View() { }

  void            initRptOrietn();
  void            saveRptOrietn();

  virtual BOOL    PreCreateWindow(CREATESTRUCT& cs);
  virtual void    OnInitialUpdate();

  virtual void    onDisplayOutput();
  virtual void    displayHeader(DevBase& dev);
  virtual void    displayFooter(DevBase& dev);

  virtual void    onPreparePrinting(CPrintInfo* info);
  virtual void    onBeginPrinting();
  virtual void    printHeader(DevBase& dev, int pageNo);
  virtual void    printFooter(DevBase& dev, int pageNo);
  virtual void    OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

  virtual void    setWidwScroll(bool scroll = true) {scrollWdw = scroll;}

  SFTPv2Doc*      GetDocument() const;


public:

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:

  DECLARE_MESSAGE_MAP()

  afx_msg void onOptions();
  afx_msg void onRptOrietn();

  afx_msg void OnSetFocus(CWnd* pOldWnd);

  afx_msg void OnLButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

  afx_msg void OnContextMenu(  CWnd* ,      CPoint point);
  afx_msg void onCopy();
  afx_msg void onPup1();
  afx_msg void onPup2();
  };



#ifndef _DEBUG  // debug version in SFTPv2View.cpp
inline SFTPv2Doc* SFTPv2View::GetDocument() const
                                           {return reinterpret_cast<SFTPv2Doc*>(m_pDocument);}
#endif




///---------------

#ifdef Examples
StoreRpt   dspStore;
StoreRpt   prtStore;
#endif
#ifdef Examples
  StoreRpt&       storeRpt()  {return dspStore;}
#endif

