#if !defined(REMOTESOCKETSERVER_H)
#define REMOTESOCKETSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerSocket.h : header file
//

//#include "Afxsock.h"
#include "RemoteSocketClient.h"
#include "VM_Sock.h"


/////////////////////////////////////////////////////////////////////////////
// CRemoteSocketServer command target

class CRemoteSocketServer : public CSocket
{
// Attributes
public:

// Operations
public:
	CRemoteSocketServer();
	CRemoteSocketServer(LPVOID pDlg);
	virtual ~CRemoteSocketServer();

	void SendSockRemoteSeatMessage(unsigned char seat, SockRemotePacket* SockPacket);

	void SendSockRemoteMessage(SockVMPacket* SockPacket);

	void SendSockRemoteMessageToRTS(SockRemotePacket* SockPacket);

// Overrides
public:

	CDialog* pDlg;
	CRemoteSocketClient m_RemoteClient[MAX_VOTE_ARRAY];
	//int remoteIndex;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteSocketServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CRemoteSocketServer)
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

#endif // !defined(REMOTESOCKETSERVER_H)
