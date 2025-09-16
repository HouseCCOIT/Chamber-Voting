#if !defined(VMSOCKETSERVER_H)
#define VMSOCKETSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VMSocketServer.h : header file
//

//#include "Afxsock.h"
#include "VMSocketClient.h"
#include "VM_Sock.h"


/////////////////////////////////////////////////////////////////////////////
// CVMSocketServer command target

class CVMSocketServer : public CSocket
{
// Attributes
public:

// Operations
public:
	CVMSocketServer();
	CVMSocketServer(LPVOID pDlg);
	virtual ~CVMSocketServer();

	void SendVMClientMessage(SockPacket* SockPacket);

// Overrides
public:

	CDialog* pDlg;
	CVMSocketClient m_VMClient[MAX_VM_SOCKETS]; // LCD Console, Speaker, Video, Real Time View, Callin

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVMSocketServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CVMSocketServer)
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

#endif // !defined(VMSOCKETSERVER_H)
