
// RealTimeView.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "RealTimeView.h"
#include "RealTimeViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRTVApp

BEGIN_MESSAGE_MAP(CRTVApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

BOOL m_TestMode = FALSE;

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
	if (bFlag && _stricmp(pszParam, "TestMode") == 0) {
		(BOOL)m_TestMode = TRUE;
	}
	else {
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	}
}

// CRTVApp construction

CRTVApp::CRTVApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CRTVApp object

CRTVApp theApp;


// CRTVApp initialization

BOOL CRTVApp::InitInstance()
{
	const char szUniqueNamedMutex[] = "com_mhr_apps_rtv";
	HANDLE hHandle = CreateMutex(NULL, TRUE, szUniqueNamedMutex);
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		// Program already running somewhere
		AfxMessageBox("Realtime View already running.");

		return(1); // Exit program
	}

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// Parse command line using custom class.
	CMyCmdLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CRTVDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Upon app closing:
	ReleaseMutex(hHandle); // Explicitly release mutex
	CloseHandle(hHandle); // close handle before terminating

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

