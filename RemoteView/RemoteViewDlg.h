
// RemoteViewDlg.h : header file
//

#pragma once


// CRemoteViewDlg dialog
class CRemoteViewDlg : public CDialog
{
// Construction
public:
	CRemoteViewDlg(CWnd* pParent = nullptr);	// standard constructor

	void JaLoadParametersAndSeating(void);
	FILETIME JaGetFileTime(CString fName);

	BOOL JaRAFileChanged();
	void JaReadRAFile(void);
	void Connect();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTEVIEW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_Exit;
	CListBox m_RemoteActiveList;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
};
