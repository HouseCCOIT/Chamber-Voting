
// RealTimeViewDlg.h : header file
//

#include "JaStatic.h"
#include "JaGB.h"
#include "ClientSocket.h"
#include "VM_Sock.h"
#include "ResultsView.h"

#pragma once

typedef BOOL(WINAPI* lpfnSetLayeredWindowAttributes)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

// CRTVDlg dialog
class CRTVDlg : public CDialog
{
// Construction
public:
	CRTVDlg(CWnd* pParent = nullptr);	// standard constructor

	void SetMemberVote(int Member, int vote);

   void ConnectToVM();

	void ShowRealTime();
	void ShowResults();
	void UpdateCurrentVote(SockPacket votePacket);
	void FadeIn();
	void FadeOut();
	BOOL m_FadeOut;
	int  m_FadeLevel;
	BOOL m_Fading;

	CClientSocket m_Client;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIMEVIEW_DIALOG };


#endif

	CJaStatic m_BorderTop;
	CJaStatic m_BorderBottom;
	CStatic   m_MemberYes[150];
	CStatic   m_MemberNo[150];
	CJaStatic m_MemberName[150];
	CJaGB     m_MemberGroupBox[5];

	CString	m_ServerIPAddress;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void JaReadCurrentVote(CString voteFilename);
	FILETIME JaGetFileTime(CString fName);
	BOOL JaVoteFileChanged();
	void JaLoadParametersAndSeating(void);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	int m_NoColor;
	int m_YesColor;
	CFont m_RTFont;
	CFont m_RTFontThin;
	int m_BackColor;
	int m_TextColor;
	int m_GBColor;

	CRVDlg* pResultsViewDlg;
	lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
