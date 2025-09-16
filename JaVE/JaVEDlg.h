// JaVEDlg.h : header file
//

#if !defined(AFX_JAVEDLG_H__64078A8F_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_)
#define AFX_JAVEDLG_H__64078A8F_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CJaVEDlg dialog

class CJaVEDlg : public CDialog
{
// Construction
public:
    CJaVEDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
    //{{AFX_DATA(CJaVEDlg)
	enum { IDD = IDD_JAVE_DIALOG };
	CEdit	m_EditEdited;
	CButton	m_RadioAbsent;
	CButton	m_RadioNo;
	CButton	m_RadioYes;
	CComboBox	m_ComboName;
	CEdit	m_EditOrderOfBusiness;
	CEdit	m_EditFilesAndResolutions;
	CEdit	m_EditDate;
	CEdit	m_EditAction;
	CComboBox	m_ComboSequence;
	int		m_JA_Vote;
	//}}AFX_DATA

	int		m_JA_Index;

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CJaVEDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CJaVEDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnRadioYes();
    afx_msg void OnRadioNo();
    afx_msg void OnRadioNovote();
    afx_msg void OnSelchangeComboName();
	afx_msg void OnSelchangeComboSequence();
	afx_msg void OnButtonSend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetfocusEditEdited();
	virtual void OnCancel();
	afx_msg void OnButtonReset();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAVEDLG_H__64078A8F_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_)
