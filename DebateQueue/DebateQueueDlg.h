
// DebateQueueDlg.h : header file
//

#pragma once

#include "afxbutton.h"
#include "stdafx.h"
#include "house.h"
#include "ClientSocket.h"

// CDebateQueueDlg dialog
class CDebateQueueDlg : public CDialog
{
// Construction
public:
	CDebateQueueDlg(CWnd* pParent = nullptr);	// standard constructor

	void JaLoadParametersAndSeating(void);
	FILETIME JaGetFileTime(CString fName);
	BOOL JaCallInFileChanged();
	void JaReadCallIns(void);
	BOOL JaDQFileChanged();
	void JaReadDQFile(void);
	void Connect();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEBATEQUEUE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CClientSocket m_Client;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox   m_StationList;
	CListBox   m_RTSList;
	CMFCButton m_ButtonAdd;
	CMFCButton m_ButtonRemove;
	CMFCButton m_ButtonExit;
	CStatic    m_CallInNames;
	CStatic    m_RTS;

	CFont m_Elephant;
	CFont m_LucidaSans;
	CFont m_Calibri;

	CBrush m_Brush;

	CString	m_ServerIPAddress;
	int      m_index;
	int		m_Seat;
	int		m_Selection;
	int		m_SeatRTS;
	int		m_SelectionRTS;
	CString  m_Name;
	CString  m_NameRTS;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnLbnSelchangeListStations();
	afx_msg void OnLbnSelchangeListRTS();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedCancel();
};
