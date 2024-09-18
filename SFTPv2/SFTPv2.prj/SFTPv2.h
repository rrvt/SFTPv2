// SFTPv2.h : main header file for the SFTPv2 application

#pragma once
#include "CApp.h"
#include "MainFrame.h"

class SFTPv2Doc;
class SFTPv2View;


class SFTPv2 : public CApp {

String roamPath;
String appPath;

public:

               SFTPv2() noexcept : CApp(this) { }

  virtual BOOL InitInstance();
  virtual int  ExitInstance();

          SFTPv2Doc*       doc()         {return (SFTPv2Doc*)  CApp::getDoc();}
          SFTPv2View*      view()        {return (SFTPv2View*) CApp::getView();}
          MainFrame*       mainFrm()     {return (MainFrame*)       m_pMainWnd;}
          String&          roamingPath() {return roamPath;}
          String&          thisAppPath() {return appPath;}

  DECLARE_MESSAGE_MAP()

  afx_msg void onAppAbout();
  afx_msg void onHelp();
  };


extern SFTPv2 theApp;

inline void             invalidate() {theApp.invalidate();}
inline SFTPv2Doc*       doc()        {return theApp.doc();}
inline SFTPv2View*      view()       {return theApp.view();}
inline MainFrame*       mainFrm()    {return theApp.mainFrm();}
inline MyToolBar&       getToolBar() {return mainFrm()->getToolBar();}

