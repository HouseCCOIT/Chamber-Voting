// JaMaint.h : main header file for the JAMAINT application
//

#if !defined(AFX_JAMAINT_H__F274FD37_39CF_11D2_9F4C_00104B9A2A27__INCLUDED_)
#define AFX_JAMAINT_H__F274FD37_39CF_11D2_9F4C_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "VM_Sock.h"


// Extended member seating structure 
typedef struct ex_lex {
    char name[31];
    short seat;
    char district[4];
    char party[33];
	char county[33];
} EXTENDED_LEX;


extern CString  m_JA_VPTitle1;
extern CString  m_JA_VPTitle2;
extern CString  m_JA_VPTitle3;
extern UINT     m_JA_SequenceNumber;
extern UINT     m_JA_SequenceNumberNew;
extern UINT     m_JA_SequenceNumberStart;
extern UINT     m_JA_SequenceNumberEnd;
extern UINT     m_JA_SessionNumber;
extern INT      m_JA_SpecialSession;
extern INT      m_JA_ToDiskette;
extern UINT     m_JA_DeleteFromSource;
extern CString  m_JA_SeatingDate;
extern BOOL     m_JA_ScratchFile;

extern EXTENDED_LEX m_JA_MemberSeatingListOld[MAX_SEATS];
extern EXTENDED_LEX m_JA_MemberSeatingListNew[MAX_SEATS];
extern char m_JA_PartyList[3][33];

/////////////////////////////////////////////////////////////////////////////
// CJaMaintApp:
// See JaMaint.cpp for the implementation of this class
//

class CJaMaintApp : public CWinApp
{
public:
    CJaMaintApp();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CJaMaintApp)
    public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation

    //{{AFX_MSG(CJaMaintApp)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAMAINT_H__F274FD37_39CF_11D2_9F4C_00104B9A2A27__INCLUDED_)
