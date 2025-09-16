#if !defined(AFX_VMCLIENTSOCKET_H)
#define AFX_VMCLIENTSOCKET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VMClientSocket.h : header file
//
#include "Afxsock.h"

/////////////////////////////////////////////////////////////////////////////
// CVMClientSocket command target

class CVMClientSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CVMClientSocket();
	virtual ~CVMClientSocket();

// Overrides
public:
	CDialog * pDlg;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVMClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CVMClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VMCLIENTSOCKET_H)
