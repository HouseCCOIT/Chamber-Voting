#if !defined(AFX_LOCKS_H__54066980_7445_11D2_AD4C_00A024C38E5B__INCLUDED_)
#define AFX_LOCKS_H__54066980_7445_11D2_AD4C_00A024C38E5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Locks.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Locks dialog

class Locks : public CDialog
{
// Construction
public:
	Locks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Locks)
	enum { IDD = IDD_DIALOG_LOCKS };
	CCheckListBox	m_LockList;
	int		m_Summary;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Locks)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Locks)
	afx_msg void OnSelchangeListLocks();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioSummary();
	afx_msg void OnRadioFullList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCKS_H__54066980_7445_11D2_AD4C_00A024C38E5B__INCLUDED_)
