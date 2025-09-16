// JaMaintDlg.h : header file
//

#if !defined(AFX_JAMAINTDLG_H__F274FD39_39CF_11D2_9F4C_00104B9A2A27__INCLUDED_)
#define AFX_JAMAINTDLG_H__F274FD39_39CF_11D2_9F4C_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CJaMaintDlg dialog

class CJaMaintDlg : public CDialog
{
// Construction
public:
    CJaMaintDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
    //{{AFX_DATA(CJaMaintDlg)
	enum { IDD = IDD_JAMAINT_DIALOG };
	CButton	m_ScratchFile;
	//}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CJaMaintDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   //{{AFX_MSG(CJaMaintDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnDeltaposSpinJaSequencenumber(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposSpinJaSessionnumber(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnKillfocusEditJaSequencenumber();
   afx_msg void OnKillfocusEditJaSessionnumber();
   afx_msg void OnButtonChangeMemberSeating();
   afx_msg void OnButtonChangePrintoutTitle();
   afx_msg void OnButtonTransferRollCall();
   afx_msg void OnButtonNewMemberSeating();
	afx_msg void OnCheckScratch();
	afx_msg void OnButtonMbOptions();
	afx_msg void OnDeltaposSpinJaSpecialsession(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditJaSpecialsession();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAMAINTDLG_H__F274FD39_39CF_11D2_9F4C_00104B9A2A27__INCLUDED_)
