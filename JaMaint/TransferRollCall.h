#if !defined(AFX_TRANSFERROLLCALL_H__9D0A85D2_3F78_11D2_9F53_00104B9A2A27__INCLUDED_)
#define AFX_TRANSFERROLLCALL_H__9D0A85D2_3F78_11D2_9F53_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TransferRollCall.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransferRollCall dialog

class CTransferRollCall : public CDialog
{
	int m_radioSelected;

// Construction
public:
	CTransferRollCall(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransferRollCall)
	enum { IDD = IDD_DIALOG_TRANSFER_ROLL_CALL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferRollCall)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransferRollCall)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioToDiskette();
	afx_msg void OnRadioFromDiskette();
	afx_msg void OnDeltaposSpinJaSequencenumberEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinJaSequencenumberStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditJaSequencenumberEnd();
	afx_msg void OnKillfocusEditJaSequencenumberStart();
	afx_msg void OnCheckDeleteFromSource();
	afx_msg void OnButtonTransfer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERROLLCALL_H__9D0A85D2_3F78_11D2_9F53_00104B9A2A27__INCLUDED_)
