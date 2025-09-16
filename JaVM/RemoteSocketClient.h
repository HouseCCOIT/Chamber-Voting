#if !defined(REMOTETSOCKETCLIENT_H)
#define REMOTETSOCKETCLIENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoteSocketClient.h : header file
//
#include "Afxsock.h"
//#include "RemoteSocketServer.h"

/////////////////////////////////////////////////////////////////////////////
// CRemoteSocketClient command target

class CRemoteSocketClient : public CSocket
{
// Attributes
public:

// Operations
public:
	CRemoteSocketClient();
	virtual ~CRemoteSocketClient();

// Overrides
public:
	CDialog* pDlg;
	CSocket* VMServer;

	char          m_Index;
	unsigned char m_Seat;
	unsigned char m_Vote;
	//char			  m_VoteOpen;

	void SendVoteOpen(int voteOpen);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteSocketClient)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CRemoteSocketClient)
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

#endif // !defined(REMOTETSOCKETCLIENT_H)
