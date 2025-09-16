#if !defined(MBSOCKETCLIENT_H)
#define MBSOCKETCLIENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MBSocketClient.h : header file
//
#include "Afxsock.h"
//#include "MBSocketServer.h"

/////////////////////////////////////////////////////////////////////////////
// CMBSocketClient command target

class CMBSocketClient : public CSocket
{
// Attributes
public:

// Operations
public:
	CMBSocketClient();
	virtual ~CMBSocketClient();

// Overrides
public:
	CDialog* pDlg;
	CSocket* VMServer;

	int m_Index;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMBSocketClient)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMBSocketClient)
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

#endif // !defined(MBSOCKETCLIENT_H)
