
// LCDDisplayDlg.h : header file
//

#include "ClientSocket.h"
#include "VM_Sock.h"

#pragma once


// CLCDDisplayDlg dialog
class CLCDDisplayDlg : public CDialogEx
{
// Construction
public:
	CLCDDisplayDlg(CWnd* pParent = nullptr);	// standard constructor

	void UpdateCurrentVote(SockPacket votePacket);

	void ConnectToVM();
	CClientSocket m_Client;
	CString	m_ServerIPAddress;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LCDDISPLAY_DIALOG };
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
	CStatic mYEA;
	CStatic mN_V;
	CStatic mNAY;
};
