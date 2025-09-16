#if !defined(VIDEOSOCKETCLIENT_H)
#define VIDEOSOCKETCLIENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoSocketClient.h : header file
//
#include "Afxsock.h"
//#include "VideoSocketServer.h"

/////////////////////////////////////////////////////////////////////////////
// CVideoSocketClient command target

class CVideoSocketClient : public CSocket
{
// Attributes
public:

// Operations
public:
	CVideoSocketClient();
	virtual ~CVideoSocketClient();

	void SendVoteOpen(int voteOpen);

// Overrides
public:
	CDialog* pDlg;
	CSocket* VMServer;

	int m_Index;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoSocketClient)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CVideoSocketClient)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
public:
	virtual void OnClose(int nErrorCode);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(VIDEOSOCKETCLIENT_H)
