// Digital Bank.h : main header file for the DIGITAL BANK application
//

#if !defined(AFX_DIGITALBANK_H__B1192D07_FBE7_11D2_9707_080009ABC65D__INCLUDED_)
#define AFX_DIGITALBANK_H__B1192D07_FBE7_11D2_9707_080009ABC65D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDigitalBankApp:
// See Digital Bank.cpp for the implementation of this class
//

class CDigitalBankApp : public CWinApp
{
public:
	CDigitalBankApp();
  
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDigitalBankApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDigitalBankApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIGITALBANK_H__B1192D07_FBE7_11D2_9707_080009ABC65D__INCLUDED_)
