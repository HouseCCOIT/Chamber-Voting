// JaMB.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "JaMB.h"
#include "JaMBDlg.h"
#include "MBThread.h"

#ifdef _DEBUG
   #define new DEBUG_NEW
   #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL m_NoWeb = FALSE;
BOOL m_TestMode = FALSE;
BOOL m_NoBounds = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CJaMBApp

BEGIN_MESSAGE_MAP(CJaMBApp, CWinApp)
//{{AFX_MSG_MAP(CJaMBApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG
ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//////////////////
// Custom class to parse command line switches.
// To parse your own switches, just override 
// CCommandLineInfo::ParseParam.
class CMyCmdLineInfo : public CCommandLineInfo {
public:
   virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast);
   enum {
      ShellNew=100
   };
};

//////////////////
// Custom override to parse our own switches.
// Recognizes /NoWeb
//
void CMyCmdLineInfo::ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
{
   if (bFlag && _stricmp(pszParam, "NoWeb") == 0) {
      (BOOL)m_NoWeb = TRUE;
   }
   else if (bFlag && _stricmp(pszParam, "TestMode") == 0) {
      (BOOL)m_TestMode = TRUE;
   }
   else if (bFlag && _stricmp(pszParam, "NoBounds") == 0) {
      (BOOL)m_NoBounds = TRUE;
   }
   else {
      CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CJaMBApp construction

CJaMBApp::CJaMBApp()
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CJaMBApp object

CJaMBApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CJaMBApp initialization

BOOL CJaMBApp::InitInstance()
{
   HWND existingApp = FindWindow(0, "Message Board");
   if (existingApp) {
      SetForegroundWindow(existingApp);
      return FALSE;
   }

   if (!AfxSocketInit()) {
      AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
      return FALSE;
   }

   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.

#ifdef _AFXDLL
   Enable3dControls();         // Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

   // Parse command line. Use my own custom class.
   CMyCmdLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   CJaMBDlg dlg;

   BOOL allIsOk;

   int nResponse = dlg.DoModal();
   if (nResponse == IDOK) {
      allIsOk = TRUE;
   }
   else if (nResponse == IDCANCEL) {
      allIsOk = FALSE;
   }

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.
   return FALSE;
}
