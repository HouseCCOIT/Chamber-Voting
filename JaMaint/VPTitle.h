#if !defined(AFX_VPTITLE_H__9D0A85D0_3F78_11D2_9F53_00104B9A2A27__INCLUDED_)
#define AFX_VPTITLE_H__9D0A85D0_3F78_11D2_9F53_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VPTitle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVPTitle dialog

class CVPTitle : public CDialog
{
// Construction
public:
	CVPTitle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVPTitle)
	enum { IDD = IDD_VP_TITLE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVPTitle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVPTitle)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VPTITLE_H__9D0A85D0_3F78_11D2_9F53_00104B9A2A27__INCLUDED_)
