
// JaCallInDlg.h : header file
//

#include "ClientSocket.h"
#include "VMClientSocket.h"
#include "VM_Sock.h"
#include "afxbutton.h"
#include "JaStatic.h"
#include "Crypto.h"
#include "house.h"
#include "listboxch.h"

#pragma once


// CJaCallInDlg dialog
class CJaCallInDlg : public CDialog
{
// Construction
public:
	CJaCallInDlg(CWnd* pParent = nullptr);	// standard constructor

	CClientSocket m_Client;
	CVMClientSocket m_VMClient;

	void UpdateCurrentVote(SockPacket votePacket);

	void UpdateButtons(SockRemotePacket* SockData);
	void JaLoadParametersAndSeating(void);
	FILETIME JaGetFileTime(CString fName);
	BOOL JaCallInFileChanged();
	void JaReadCallIns(void);
	BOOL JaRemoteFileChanged();
	void JaReadRemotes(void);
	BOOL JaVoteFileChanged();
	void JaReadCurrentVote();
	BOOL JaLockFileChanged();
	void JaReadLocks();
	void Connect();
	void Disconnect();
	void ConnectToVM();

	void JaWriteRemotes(CString remoteFilename);
	void JaWriteCallIns(CString callinFilename);

	CString	m_ServerIPAddress;
	int      m_index;
	int		m_Seat;
	int		m_Selection;
	CString  m_Name;
	bool     m_VoteOpen = false;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JACALLIN_DIALOG };
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
   afx_msg void OnBnClickedButtonYea();
   afx_msg void OnBnClickedButtonNay();
   afx_msg void OnBnClickedButtonReset();
   afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonPrint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonUpdate();

	void ButtonReset();

	CMFCButton m_ButtonYea;
	CMFCButton m_ButtonNay;
	CMFCButton m_ButtonReset;
	CMFCButton m_ButtonPage;
	CMFCButton m_ButtonLEDYea;
	CMFCButton m_ButtonLEDNay;
	CMFCButton m_ButtonLEDReset;
	CMFCButton m_ButtonLEDPage;
	CMFCButton m_ButtonExit;
	CMFCButton m_ButtonUpdate;
	CMFCButton m_ButtonPrint;

	CJaStatic m_FullName;
	CFont m_Elephant;
	CFont m_LucidaSans;
	CFont m_Calibri;

	CBrush m_Brush;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CListBox m_StationList;
	//CListBoxCH m_StationList;
	afx_msg void OnLbnSelchangeListStations();
	CStatic m_CallInNames;
};
