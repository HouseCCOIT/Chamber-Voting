#if !defined(VIDEOSOCKETSERVER_H)
#define VIDEOSOCKETSERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoSocketServer.h : header file
//

//#include "Afxsock.h"
#include "VideoSocketClient.h"
#include "VM_Sock.h"


/////////////////////////////////////////////////////////////////////////////
// CVideoSocketServer command target

class CVideoSocketServer : public CSocket
{
// Attributes
public:

// Operations
public:
	CVideoSocketServer();
	CVideoSocketServer(LPVOID pDlg);
	virtual ~CVideoSocketServer();

	void SendVideoClientMessage(SockPacket* SockPacket);

// Overrides
public:

	CDialog* pDlg;
	CVideoSocketClient m_VideoClient[3]; // RealTimeView and JaVideo

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoSocketServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CVideoSocketServer)
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

#endif // !defined(VIDEOSOCKETSERVER_H)
