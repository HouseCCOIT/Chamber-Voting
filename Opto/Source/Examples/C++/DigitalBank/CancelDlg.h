#if !defined(AFX_CANCELDLG_H__0CE6E7E0_FC78_11D2_9707_080009ABC65D__INCLUDED_)
#define AFX_CANCELDLG_H__0CE6E7E0_FC78_11D2_9707_080009ABC65D__INCLUDED_

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

// CancelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCancelDlg dialog

class CCancelDlg : public CDialog
{
// Construction
public:
	CCancelDlg(CWnd* pParent = NULL);   // standard constructor

  BOOL m_bCancel;


// Dialog Data
	//{{AFX_DATA(CCancelDlg)
	enum { IDD = IDD_CANCEL_OPEN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCancelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCancelDlg)
	afx_msg void OnCancelBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANCELDLG_H__0CE6E7E0_FC78_11D2_9707_080009ABC65D__INCLUDED_)
