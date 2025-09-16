
// JaStressDlg.h : header file
//

#include "ClientSocket.h"
#include "VM_Sock.h"
#include "afxbutton.h"
#include "JaStatic.h"
#include "house.h"

#pragma once


// CJaStressDlg dialog
class CJaSperDlg : public CDialog
{
// Construction
public:
	CJaSperDlg(CWnd* pParent = nullptr);	// standard constructor

	CClientSocket m_Client[MAX_SEATS];

	void UpdateButtons(SockRemotePacket* SockData);
	void JaLoadParametersAndSeating(void);
	FILETIME JaGetFileTime(CString fName);
	BOOL JaRemoteFileChanged();
	void JaReadRemotes(void);
	void GetCurrentVote();

	CString	m_ServerIPAddress;
	int      m_index;
	int		m_Seat;
	int		m_Selection;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JASTRESS_DIALOG };
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
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonDisconnect();

	void ButtonReset();

	CMFCButton m_ButtonYea;
	CMFCButton m_ButtonNay;
	CMFCButton m_ButtonReset;
	CMFCButton m_ButtonPage;
	CMFCButton m_ButtonLEDYea;
	CMFCButton m_ButtonLEDNay;
	CMFCButton m_ButtonLEDReset;
	CMFCButton m_ButtonLEDPage;
	CMFCButton m_ButtonConnect;
	CMFCButton m_ButtonDisconnect;
	CMFCButton m_ButtonExit;

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
