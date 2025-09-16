
// JaRemoteDlg.h : header file
//

#pragma once

#include "ClientSocket.h"
#include "VM_Sock.h"
#include "afxbutton.h"
#include "JaStatic.h"
#include "Crypto.h"
#include "Nffv.h"

// CJaRemoteDlg dialog
class CJaRemoteDlg : public CDialog
{
// Construction
public:
	CJaRemoteDlg(CWnd* pParent = nullptr);	// standard constructor

	static UINT EnrollUserThread(LPVOID pParam);
	static UINT VerifyUserThread(LPVOID pParam);
	static const TCHAR* GetNffvStatusDescription(NffvStatus status);

   //void OnBnClickedButtonVerify();

	CClientSocket m_Client;

	void UpdateButtons(SockRemotePacket* SockData);

	void LoadConfig();
	void ButtonOpen();
	void ButtonReset();
	bool Verify();

	void GetCurrentVote();

	//	The crypto object.
	MFC::CCrypto m_crypto;

	CString m_strPassword;
	CString m_strData;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JAREMOTE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CString  m_Name;
	CString	m_ServerIPAddress;
	int      m_index;
	int		m_Seat;
	int		m_Vote;
	bool     m_VoteOpen = false;

	CString m_fpDB;
	CString m_uDB;
	CString m_pwd;
	CString m_fp;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedButtonConnect();
   afx_msg void OnBnClickedButtonYea();
   afx_msg void OnBnClickedButtonNay();
   afx_msg void OnBnClickedButtonReset();
   afx_msg void OnBnClickedButtonPage();
	afx_msg void OnBnClickedButtonRts();
   afx_msg void OnBnClickedButtonVerify();
	afx_msg void OnBnClickedButtonVerify2();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnScanCancel();
	afx_msg LRESULT OnBusyFormFinish(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonConnection();

	CMFCButton m_ButtonYea;
	CMFCButton m_ButtonNay;
	CMFCButton m_ButtonReset;
	CMFCButton m_ButtonPage;
	CMFCButton m_ButtonRTS;
	CMFCButton m_ButtonConnection;
	CMFCButton m_ButtonLEDYea;
   CMFCButton m_ButtonLEDNay;
   CMFCButton m_ButtonLEDReset;
	CMFCButton m_ButtonLEDPage;
	CMFCButton m_ButtonLEDRTS;
	CMFCButton m_ConnectionLED;
	CMFCButton m_ButtonConnect;
	CMFCButton m_ButtonDisconnect;
	CMFCButton m_ButtonExit;
	CMFCButton m_ButtonVerify;
	CMFCButton m_ButtonVerify2;

	CJaStatic m_FullName;
	CFont m_Elephant;
	CFont m_LucidaSans;
	CFont m_LucidaSansSmall;
	CFont m_Calibri;

	CBrush m_Brush;

	CJaStatic m_VerifyMessage;
};
