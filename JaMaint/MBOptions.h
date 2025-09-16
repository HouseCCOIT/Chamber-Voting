
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMBOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MBOptions dialog

class CMBOptions : public CDialog
{
// Construction
public:
	void StoreMBOptions();
	bool LoadMBOptions();
	CMBOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMBOptions)
	enum { IDD = IDD_MBOPTIONS_DIALOG };
	CEdit	m_EditList;
	CComboBox	m_ComboListSel;
	CComboBox	m_Combo_OOB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMBOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMBOptions)
    afx_msg void OnSelchangeComboListSel();
	afx_msg void OnButtonUpdate();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_OrderOfBusinessData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

