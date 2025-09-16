#if !defined(VMSOCKETCLIENT_H)
#define VMSOCKETCLIENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VMSocketClient.h : header file
//
#include "Afxsock.h"
//#include "VMSocketServer.h"

/////////////////////////////////////////////////////////////////////////////
// CVMSocketClient command target

class CVMSocketClient : public CSocket
{
// Attributes
public:

// Operations
public:
	CVMSocketClient();
	virtual ~CVMSocketClient();

// Overrides
public:
	CDialog* pDlg;
	CSocket* VMServer;

	int m_Index;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVMSocketClient)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CVMSocketClient)
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

#endif // !defined(VMSOCKETCLIENT_H)
