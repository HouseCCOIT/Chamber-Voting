// JaMaint.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "io.h"
#include "stdio.h"
#include "JaMaint.h"
#include "JaFile.h"
#include "JaMaintDlg.h"
#include "maintain.h"
#include "VM_Sock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString m_JA_VPTitle1;
CString m_JA_VPTitle2;
CString m_JA_VPTitle3;
UINT    m_JA_SequenceNumber = 1;
UINT    m_JA_SequenceNumberNew;
UINT    m_JA_SequenceNumberStart;
UINT    m_JA_SequenceNumberEnd;
UINT    m_JA_SessionNumber;
INT     m_JA_SpecialSession;
INT     m_JA_ToDiskette = 1;
UINT    m_JA_DeleteFromSource;
CString m_JA_SeatingDate;
BOOL    m_JA_ScratchFile;

CString JaVoteIniFile  = "V:\\JaVote.Ini";
CString JaVoteIniBack  = "S:\\JaVote.Ini";
CString JaVoteScratch  = "V:\\JaScratch.Ini";
CString JaSequenceFile = "V:\\Votes\\Sequence.vot";
CString JaSequenceBack = "S:\\Votes\\Sequence.vot";

EXTENDED_LEX m_JA_MemberSeatingListOld[MAX_SEATS];
EXTENDED_LEX m_JA_MemberSeatingListNew[MAX_SEATS];
char m_JA_PartyList[3][33] = { "DFL", "IR", "Other" };

/////////////////////////////////////////////////////////////////////////////
// CJaMaintApp

BEGIN_MESSAGE_MAP(CJaMaintApp, CWinApp)
    //{{AFX_MSG_MAP(CJaMaintApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaMaintApp construction

CJaMaintApp::CJaMaintApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CJaMaintApp object

CJaMaintApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CJaMaintApp initialization

BOOL CJaMaintApp::InitInstance()
{
   SetJaSequenceFilename(JaSequenceFile);
   JaLoadSequenceNumber();  // Load sequence number

   SetJaVoteIniFilename(JaVoteIniFile);
	m_JA_ScratchFile = FALSE;

    // Does JaScratch.Ini exist?
    if ((_access(JaVoteScratch, 0 )) != -1 ) {
       if (AfxMessageBox("A Scratch File exists.Do you want to use it.  ", MB_YESNO) == IDYES) {
          SetJaVoteIniFilename(JaVoteScratch);
          m_JA_ScratchFile = TRUE;

          JaLoadParametersAndSeating();   // Load parameters and seating
       }
       else if ((_access(JaVoteIniFile, 0 )) != -1 ) { // Does JaVote.Ini exist?
          JaLoadParametersAndSeating();   // Load parameters and seating
       }
       else {
          JaLoadParmFile();
          JaLoadSeating();
	   }
    }
    else if ((_access(JaVoteIniFile, 0 )) != -1 ) { // Does JaVote.Ini exist?
       JaLoadParametersAndSeating();   // Load parameters and seating
	}
    else {
       JaLoadParmFile();
       JaLoadSeating();
	}


    Enable3dControlsStatic();   // Call this when linking to MFC statically

    CJaMaintDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
		if (m_JA_ScratchFile) {
           SetJaVoteIniFilename(JaVoteScratch);

           JaStoreParameters();
           JaStoreSeating();
		}
		else {
           SetJaSequenceFilename(JaSequenceFile);
           SetJaVoteIniFilename(JaVoteIniFile); 

           JaStoreSequenceNumber();
           JaStoreParameters();
           JaStoreSeating();
        
           SetJaSequenceFilename(JaSequenceBack);
           SetJaVoteIniFilename(JaVoteIniBack);
        
           JaStoreSequenceNumber();
           JaStoreParameters();
           JaStoreSeating();
		}
    }
    else if (nResponse == IDCANCEL) {
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}


