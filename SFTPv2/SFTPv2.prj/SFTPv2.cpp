// SFTPv2.cpp : Defines the class behaviors for the application.


#include "pch.h"
#include "SFTPv2.h"
#include "AboutDlg.h"
#include "FileName.h"
#include "IniFile.h"
#include "NotePad.h"
#include "Resource.h"
#include "SFTPv2Doc.h"
#include "SFTPv2View.h"


SFTPv2  theApp;                       // The one and only SFTPv2 object
IniFile iniFile;


// SFTPv2

BEGIN_MESSAGE_MAP(SFTPv2, CWinAppEx)
  ON_COMMAND(ID_Help,      &onHelp)
  ON_COMMAND(ID_App_About, &onAppAbout)
END_MESSAGE_MAP()


// SFTPv2 initialization

BOOL SFTPv2::InitInstance() {

  CWinAppEx::InitInstance();

  iniFile.setAppDataPath(m_pszHelpFilePath, *this);

  roamPath = getPath(iniFile.getAppDataPath(m_pszHelpFilePath));
  appPath  = getPath(m_pszHelpFilePath);

  notePad.clear();

  SetRegistryKey(appID);

  LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

  // Register the application's document templates.  Document templates
  // serve as the connection between documents, frame windows and views

  CSingleDocTemplate* pDocTemplate;

  pDocTemplate = new CSingleDocTemplate(
    IDR_MAINFRAME,
    RUNTIME_CLASS(SFTPv2Doc),
    RUNTIME_CLASS(MainFrame),       // main SDI frame window
    RUNTIME_CLASS(SFTPv2View));

  if (!pDocTemplate) return FALSE;

  AddDocTemplate(pDocTemplate);

  // Parse command line for standard shell commands, DDE, file open

  CCommandLineInfo cmdInfo;   ParseCommandLine(cmdInfo);

  // Dispatch commands specified on the command line.  Will return FALSE if
  // app was launched with /RegServer, /Register, /Unregserver or /Unregister.

  if (!ProcessShellCommand(cmdInfo)) return FALSE;

  setAppName(_T("Secure File Transfer Protocol v2")); setTitle(_T("Main"));

  view()->setFont(_T("Arial"), 12.0);

  doc()->defaultSite();

  m_pMainWnd->ShowWindow(SW_SHOW);   m_pMainWnd->UpdateWindow();   return TRUE;
  }



int SFTPv2::ExitInstance() {

#ifdef DebugMemoryLeaks
  _CrtDumpMemoryLeaks();
#endif

  return CApp::ExitInstance();
  }


void SFTPv2::onHelp() {
String topic = m_pszHelpFilePath; topic += _T(">Introduction");

  if (doc()->isLocked()) return;

  ::HtmlHelp(m_pMainWnd->m_hWnd, topic,  HH_DISPLAY_TOC, 0);
  }


void SFTPv2::onAppAbout() {

  if (doc()->isLocked()) return;

  AboutDlg aboutDlg; aboutDlg.DoModal();
  }

