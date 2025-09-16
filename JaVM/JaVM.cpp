// JaVM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "JaVM.h"
#include "JaVMDlg.h"
#include "VMThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL m_TestMode = FALSE;

CString m_RTFontName = "Arial Bold";
int     m_RTFontSize = 110;
int     m_Delay = 50;

/////////////////////////////////////////////////////////////////////////////
// CJaVMApp

BEGIN_MESSAGE_MAP(CJaVMApp, CWinApp)
   //{{AFX_MSG_MAP(CJaVMApp)
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
      ShellNew = 100
   };
};

//////////////////
// Custom override to parse our own switches.
// Recognizes /TestMode
//
void CMyCmdLineInfo::ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
{
   if (bFlag && _stricmp(pszParam, "A") == 0) {
      m_RTFontName = "Arial";
      m_RTFontSize = 100;
   }
   else if (bFlag && _stricmp(pszParam, "T") == 0) {
      m_RTFontName = "Times New Roman";
      m_RTFontSize = 100;
   }
   else if (bFlag && _stricmp(pszParam, "H") == 0) {
      m_RTFontName = "Helvetica";
      m_RTFontSize = 100;
   }
   else if (bFlag && _stricmp(pszParam, "TestMode") == 0) {
      (BOOL)m_TestMode = TRUE;
   }
   else if (bFlag && _strnicmp(pszParam, "Delay", 5) == 0) {
      char tmpStr[16];
      strcpy_s(tmpStr, pszParam + 5);
      m_Delay = atoi(tmpStr);
   }
   else
      CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}

/////////////////////////////////////////////////////////////////////////////
// CJaVMApp construction

CJaVMApp::CJaVMApp()
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CJaVMApp object

CJaVMApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CJaVMApp initialization

BOOL CJaVMApp::InitInstance()
{
   const char szUniqueNamedMutex[] = "com_mhr_apps_vm";
   HANDLE hHandle = CreateMutex(NULL, TRUE, szUniqueNamedMutex);
   if (ERROR_ALREADY_EXISTS == GetLastError()) {
      // Program already running somewhere
      AfxMessageBox("Voting Machine already running.");
      return(1); // Exit program
   }

   if (!AfxSocketInit()) {
      AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
      return FALSE;
   }

   AfxEnableControlContainer();

   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.

#ifdef _AFXDLL
   //Enable3dControls();         // Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

   // Parse command line using custom class.
   CMyCmdLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   CJaVMDlg dlg;
   m_pMainWnd = &dlg;
   int nResponse = dlg.DoModal();
   if (nResponse == IDOK) {
   }
   else if (nResponse == IDCANCEL) {
   }

   // Upon app closing:
   ReleaseMutex(hHandle); // Explicitly release mutex
   CloseHandle(hHandle); // close handle before terminating

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.
   return FALSE;
}
