// MainFrame.h : interface of the MainFrame class


#pragma once
#include "CMainFrm.h"
#include "MyToolBar.h"

#include "Site.h"
#include "WinPos.h"


class MainFrame : public CMainFrm {

CMFCMenuBar   menuBar;
MyToolBar     toolBar;
CMFCStatusBar statusBar;
CProgressCtrl progressBar;

bool          isInitialized;

protected:                                          // create from serialization only

  MainFrame() noexcept;

  DECLARE_DYNCREATE(MainFrame)

public:                                             // Overrides

  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  virtual     ~MainFrame();

  MyToolBar&   getToolBar() {return toolBar;}
  void         startPrgBar(int noSteps);
  void         closePrgBar() {progressBar.DestroyWindow();}

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:                                          // Generated message map functions

  DECLARE_MESSAGE_MAP()

  afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg LRESULT OnResetToolBar(WPARAM wParam, LPARAM lParam);

private:

  void            setupToolBar();

public:

  afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);

  afx_msg void    OnMove(int x, int y);
  afx_msg void    OnSize(UINT nType, int cx, int cy);

  afx_msg LRESULT onStepPrgBarMsg(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT onUpdateMsg(    WPARAM wParam, LPARAM lParam)
                                                          {return site.finUpdate(wParam, lParam);}
  afx_msg LRESULT onSetWdwScroll( WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT onDisplayMsg(   WPARAM wParam, LPARAM lParam);
  };


