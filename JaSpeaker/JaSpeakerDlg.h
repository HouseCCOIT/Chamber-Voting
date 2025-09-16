
// JaSpeakerDlg.h : header file
//

#include "house.h"
#include "JaRound.h"
#include "JaStatic.h"
#include "VM_Sock.h"
#include "ClientSocket.h"

#pragma once


// CJaSpeakerDlg dialog
class CJaSpeakerDlg : public CDialogEx
{
// Construction
public:
	CJaSpeakerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JASPEAKER_DIALOG };
#endif

   CJaRound  m_MemberSeat[150];

	CJaStatic	m_JD_Static14;
   CJaStatic	m_JD_Static13;
   CJaStatic	m_JD_Static12;
	CJaStatic	m_JD_Static11;
	CJaStatic	m_JD_Static10;
	CJaStatic	m_JD_Static9;
	CJaStatic	m_JD_Static8;
	CJaStatic	m_JD_Static7;
	CJaStatic	m_JD_Static6;
	CJaStatic	m_JD_Static5;
	CJaStatic	m_JD_Static3;
	CJaStatic	m_JD_Static4;
	CJaStatic	m_JD_Static2;
	CJaStatic	m_JD_Static1;

   CJaStatic	m_JD_Static_DateTime;
	CJaStatic	m_JD_Static_SpeakerLabel;

	CString	m_ServerIPAddress;
	void UpdateCurrentVote(SockPacket votePacket);
	CClientSocket m_Client;
	void ConnectToVM();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void JaLoadParametersAndSeating(void);
	void JaReadCurrentVote();
	void JaReadLocks();
	void JaReadRemotes();
	void JaReadCallIns();
	void JaReadDebateQueue();
   FILETIME JaGetFileTime(CString fName);
	BOOL JaVoteFileChanged();
   BOOL JaLockFileChanged();
	BOOL JaCallInFileChanged();
	BOOL JaRemoteFileChanged();
	BOOL JaResultsFileChanged();
	BOOL JaDQFileChanged();
	BOOL LoadDispOptions();
	void UpdateDisplay();
   void RefreshNames();

	void ClearResults();
	void SetResultLine(char *text1, char *text2, char *text3, char *text4);
	void SetLine(int i, char *text);
	void ClearAll();
	void ClearSix();
	void SetFont(CString fontName);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CFont m_SFont;

	int m_TextColor;
	int m_BackColor;
	CString m_ResultLine[5];
	CString m_DisplayLine[9];

	int m_FontSizePoints;
	CFont m_MyFont;
	CString m_FontName;
};
