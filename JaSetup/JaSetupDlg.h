
// JaSetupDlg.h : header file
//

#include "Crypto.h"
#include "Nffv.h"

#pragma once


// CJaSetupDlg dialog
class CJaSetupDlg : public CDialog
{
// Construction
public:
	CJaSetupDlg(CWnd* pParent = nullptr);	// standard constructor

	void JaLoadParametersAndSeating(void);
	static UINT EnrollUserThread(LPVOID pParam);
	static UINT VerifyUserThread(LPVOID pParam);
	static const TCHAR* GetNffvStatusDescription(NffvStatus status);

	//	The crypto object.
	MFC::CCrypto m_crypto;

	CString m_strPassword;
	CString m_strData;

	CString m_Name;
	CString m_fpDB;
	CString m_uDB;
	CString m_pwd;
	CString m_fp;

	int m_iEnrollTimeout;
	int m_iVerifyTimeout;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JASETUP_DIALOG };
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
   //CCheckListBox m_StationList;
	CListBox m_StationList;
	afx_msg void OnBnClickedButtonActivate();
	afx_msg void OnLbnSelchangeListStations();
   CEdit m_ServerIPAddress;
   CEdit m_Password;
   afx_msg void OnEnChangeEditPassword();
   CButton m_ButtonActivate;
	afx_msg void OnBnClickedButtonEnroll();
	afx_msg void OnBnClickedButtonVerify();
	afx_msg void OnBnClickedButtonEnroll2();
	afx_msg void OnBnClickedButtonVerify2();
protected:
	afx_msg LRESULT OnBusyFormFinish(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnScanCancel();
	CButton m_ButtonEnroll;
	CButton m_ButtonVerify;
	CButton m_ButtonEnroll_2;
	CButton m_ButtonVerify_2;
};
