#if !defined(MBSOCKETSERVER_H)
#define MBSOCKETSERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MBSocketServer.h : header file
//

//#include "Afxsock.h"
#include "MBSocketClient.h"
#include "VM_Sock.h"


/////////////////////////////////////////////////////////////////////////////
// CMBSocketServer command target

class CMBSocketServer : public CSocket
{
// Attributes
public:

// Operations
public:
	CMBSocketServer();
	CMBSocketServer(LPVOID pDlg);
	virtual ~CMBSocketServer();

	void SendMBClientMessage(SockPacket* SockPacket);

// Overrides
public:

	CDialog* pDlg;
	CMBSocketClient m_MBClient[3]; // RealTimeView and JaMB

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMBSocketServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMBSocketServer)
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

#endif // !defined(MBSOCKETSERVER_H)
