// JaVE.h : main header file for the JAVE application
//

#if !defined(AFX_JAVE_H__64078A8D_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_)
#define AFX_JAVE_H__64078A8D_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CJaVEApp:
// See JaVE.cpp for the implementation of this class
//

class CJaVEApp : public CWinApp
{
public:
	CJaVEApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJaVEApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CJaVEApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAVE_H__64078A8D_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_)
